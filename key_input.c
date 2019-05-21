
/*
 * key_input.c 
 */

#include <stdio.h>
#include <termio.h>

#include "gles3.h"
#include "window.h"
#include "gear.h"
#include "gpu_texture.h"
#include "demo_state.h"
#include "print_info.h"
#include "user_options.h"
#include "camera.h"
#include "shaders.h"

extern void toggle_useVSync(void);

// number of frames to draw before checking if a key on the keyboard was hit
#define FRAMES 30
 
static struct termios saved_attributes;
static int initialized = 0;

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
      set_key_down_update(light_move_y, 10.0f);
      break;
      
    case 'B': // move light down
      set_key_down_update(light_move_y, -10.0f);
      break;
      
    case 'C': // move light left
      set_key_down_update(light_move_x, 10.0f);
      break;
      
    case 'D': // move light right
      set_key_down_update(light_move_x, -10.0f);
      break;

  }
   
}

static void check_editkey(const int inpkey)
{
  //printf("input %i\n", inpkey);
  
  switch (inpkey)
  {

    case 'F': // move window off screen
      window_hide();
      break;

    case 'H': // move window to center of screen
      window_center();
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

int check_key(const int inpkey)
{
  // value returned to indicate if more keys to be processed or terminate program
  int result = FRAMES;

  //printf("input %i\n", inpkey);
  
  switch(inpkey)
  {
    case 10:
    case 13:
    case 27: // check escape sequence
      result = 0;
      break;
           
    case 'i': 
      print_GLInfo();
      break;
      
    case 'l':
      toggle_drawmode();
      break;
      
    case 'o':
      print_CLoptions_help();
      break;
    
    case 'v': 
      toggle_useVSync();
      break;

    case 'z':
      set_key_down_update(window_zoom, 10.0f);
      break;
      
    case 'Z':
      set_key_down_update(window_zoom, -10.0f);
      break;
      
    case '<':
      set_key_down_update(change_angleVel, -10.0f);
      break;
      
    case '>':
      set_key_down_update(change_angleVel, 10.0f);
      break;
      
    case 'r':
      set_key_down_update(camera_change_z, -1.0f);
      break;
          
    case 'f':
      set_key_down_update(camera_change_z, 1.0f);
      break;
      
    case 'a':
      set_key_down_update(camera_change_x, 1.0f);
      break;
          
    case 'd':
      set_key_down_update(camera_change_x, -1.0f);
      break;
          
    case 'w':
      set_key_down_update(camera_change_y, -1.0f);
      break;

    case 's':
      set_key_down_update(camera_change_y, 1.0f);
      break;
      
    case 'b':
      demo_state_toggle_VBO();
      break;
      
    case 'S':
      window_show();
      break;
      
    case 'R':
      load_shader_programs();
      break;
      
    case 'p':
      update_angleVel(0.0);
      break;
      
    case 'I':
      inc_instances();
      break;
      
    case 'O':
      dec_instances();
      break; 
          
    default: print_keyhelp();
  }
  
  return result;
}


int detect_keypress(void)
{
  int active = FRAMES;
  const int keyswaiting = _kbhit();
  
  if (keyswaiting) {
    //printf("keys waiting: %i\n", keyswaiting);
    int key = getchar();
    if (key == 27) {
      if (_kbhit()) check_esckey(getchar());
      else active = 0;
    }
    else {
      active = check_key(key);
    }
    // if falling behind in key processing then speed it up
    if (_kbhit()) active = 2; 
    move_rate_on();
  }
  else {
    // reset move_rate since no keys are being pressed
    move_rate_off();
  }
  
  return active;  
}

