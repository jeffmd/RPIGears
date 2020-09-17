// key_input.c 

#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>

#include "tasks.h"
#include "key_action.h"
#include "key_map.h"

// programmable keys ascii 0 to 127

#define KEY_END 127
#define MAX_BUTTON 3

// number of frames to draw before checking if a key on the keyboard was hit
#define FRAMES 30

typedef void (*UPDATE_KEY_DOWN)(const float);

static struct termios saved_attributes;
static int initialized = 0;

static short KeyScan_task;

static short key_map = 0;

// keyboard data
UPDATE_KEY_DOWN key_down_update; // points to a function that gets executed each time a key goes down or repeats
float rate; // the rate at which a change occurs
short rate_enabled; // if enabled the change_rate will increase each frame
float rate_direction; // direction and scale for rate change
float rate_frame; // how much the rate changes each frame

static short get_key_map(void)
{
  if (!key_map) {
    key_map = Key_Map_create();
  }

  return key_map;
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

void Key_input_action(const int inpkey)
{
  Key_Map_action(get_key_map(), inpkey, 0, 0);
}

void Key_add_action(const int key, ActionFn action, const char *help)
{
  Key_Map_add(get_key_map(), Key_Action_create(key, action, help));
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
  Key_Action_print_help();
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

static int detect_keypress(void)
{
  int active = FRAMES;
  const int keyswaiting = _kbhit();
  
  if (keyswaiting) {
    //printf("keys waiting: %i\n", keyswaiting);
    int key = getchar();
    if (key == 27) {
      if (_kbhit()) {
        //check_esckey(getchar());
      }
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
    case 2: Task_set_interval(KeyScan_task, 10); break;
    default: Task_set_interval(KeyScan_task, 100);
  }
}

static void print_help(const short souce_id, const short destination_id)
{
  Key_input_print_help();
}

void Key_input_init(void)
{
  KeyScan_task = Task_create(100, do_KeyScan_task);
  Key_input_rate_off();
  Key_add_action('h', print_help, "print help");
}

