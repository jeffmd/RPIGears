
/*
 * key_input.c 
 */

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>

#include "print_info.h"
#include "tasks.h"
#include "action_table.h"

extern void toggle_useVSync(void);

// programmable keys ascii 32 to 127

#define KEY_START 10
#define KEY_END 127
#define KEY_COUNT KEY_END - KEY_START

// number of frames to draw before checking if a key on the keyboard was hit
#define FRAMES 30
 
static struct termios saved_attributes;
static int initialized = 0;

static short KeyScan_task;

typedef void (*ActionFn)(const short souce_id, const short destination_id);
typedef void (*UPDATE_KEY_DOWN)(const float);

//static ActionFn keyActions[KEY_COUNT];
static short key_action_table=0;
static const char *keyHelp[KEY_COUNT];

// keyboard data
UPDATE_KEY_DOWN key_down_update; // points to a function that gets executed each time a key goes down or repeats
float rate; // the rate at which a change occurs
short rate_enabled; // if enabled the change_rate will increase each frame
float rate_direction; // direction and scale for rate change
float rate_frame; // how much the rate changes each frame

static short get_key_action_table(void)
{
  if (!key_action_table) {
    key_action_table = Action_table_create();
    Action_table_allocate_slots(key_action_table, KEY_COUNT);
  }

  return key_action_table;
}

static void reset_input_mode(void)
{
  if (initialized) {
    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
    fprintf (stdout, "\nstdin terminal attributes restored\n");
  }
}

// Check to see if a key has been pressed
// returns 0 if no key is being pressed
// returns >0 (number of keys in buffer) if a key has been pressed
static int _kbhit(void) {

  if (! initialized) {
    struct termios term;
    
    /* Save the terminal attributes so we can restore them later. */
    tcgetattr (STDIN_FILENO, &saved_attributes);
    atexit (reset_input_mode);
    // Use termios to turn off line buffering and echo
    tcgetattr (STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    setbuf(stdin, NULL);
    initialized = 1;
  }

  int bytesWaiting;
  ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);

  return bytesWaiting;
}


static void check_movekey(const int inpkey)
{
  //printf("input %i\n", inpkey);
  
  switch (inpkey)
  {
    case 'A': // move light up
      //set_key_down_update(light_move_y, 10.0f);
      break;
      
    case 'B': // move light down
      //set_key_down_update(light_move_y, -10.0f);
      break;
      
    case 'C': // move light left
      //set_key_down_update(light_move_x, 10.0f);
      break;
      
    case 'D': // move light right
      //set_key_down_update(light_move_x, -10.0f);
      break;

  }
   
}

static void check_editkey(const int inpkey)
{
  //printf("input %i\n", inpkey);
  
  switch (inpkey)
  {

    case 'F': // move window off screen
      //window_hide();
      break;

    case 'H': // move window to center of screen
      //window_center();
      break;
  }
  
}


static void check_esckey(const int inpkey)
{
  //printf("input %i\n", inpkey);

  switch (inpkey)
  {
    case 'O': // process edit keys
      check_editkey(getchar());
      break;

    case '[': // process arrow keys
      check_movekey(getchar());
      break;
  }
  
}

void Key_input_action(const int inpkey)
{
  if ((inpkey >= KEY_START) && (inpkey <= KEY_END)) {
     const short key_id = inpkey - KEY_START;
     Action_table_execute(get_key_action_table(), key_id, 0, 0);
  }
}

void Key_add_action(const int key, ActionFn action, const char *help)
{
  if ((key >= KEY_START) && (key <= KEY_END)) {
    const short key_id = key - KEY_START;
    Action_table_set_action(get_key_action_table(), key_id, action);
    keyHelp[key - KEY_START] = help;
  }
  else
    printf("key binding out of range: %i\n", key);
}

void Key_input_set_update(UPDATE_KEY_DOWN fn, const float val)
{
  key_down_update = fn;
  rate_direction = val;
}

void Key_input_down_update(void)
{
  if (key_down_update) {
    key_down_update(rate_direction * rate_frame);
  }
}


void Key_input_print_help(void)
{
  for (int key = 0; key < KEY_COUNT; key++) {
    if (keyHelp[key]) {
      printf("%c - %s\n", key + KEY_START, keyHelp[key]);
    }
  }  
}

void Key_input_set_rate_frame(const float period_rate)
{
  rate_frame = rate * period_rate;
}

void Key_input_rate_on(void)
{
  rate_enabled = 1;
}

void Key_input_rate_off(void)
{
  rate_enabled = 0;
  rate = 1.0f;
  key_down_update = 0;
}

void Key_input_inc_rate(void)
{
  // increase movement speed if not at max
  //if (rate_enabled) rate += 10 * state->period_rate;
}

static int detect_keypress(void)
{
  int active = FRAMES;
  const int keyswaiting = _kbhit();
  
  if (keyswaiting) {
    //printf("keys waiting: %i\n", keyswaiting);
    int key = getchar();
    if (key == 27) {
      if (_kbhit()) check_esckey(getchar());
      else Key_input_action(key);
    }
    else {
      Key_input_action(key);
    }
    // if falling behind in key processing then speed it up
    if (_kbhit()) active = 2; 
    Key_input_rate_on();
  }
  else {
    // reset move_rate since no keys are being pressed
    Key_input_rate_off();
  }
  
  return active;  
}

static void do_KeyScan_task(void)
{
  switch (detect_keypress())
  {
    // speed up key processing if more keys in buffer
    case 2: task_set_interval(KeyScan_task, 10); break;
    default: task_set_interval(KeyScan_task, 100);
  }
}

static void print_help(const short souce_id, const short destination_id)
{
  Key_input_print_help();
}

void Key_input_init(void)
{
  KeyScan_task = task_create();
  task_set_action(KeyScan_task, do_KeyScan_task);
  task_set_interval(KeyScan_task, 100);
  Key_input_rate_off();
  Key_add_action('h', print_help, "print help");
}

