
/*
 * key_input.c 
 */
 
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
    case 'A': // move window up
      set_key_down_update(move_window_y, -10.0f);
      break;
      
    case 'B': // move window down
      set_key_down_update(move_window_y, 10.0f);
      break;
      
    case 'C': // move window left
      set_key_down_update(move_window_x, 10.0f);
      break;
      
    case 'D': // move window right
      set_key_down_update(move_window_x, -10.0f);
      break;

  }
   
}

static void check_editkey(const int inpkey)
{
  //printf("input %i\n", inpkey);
  
  switch (inpkey)
  {

    case 'F': // move window off screen
      move_window_end();
      break;

    case 'H': // move window to center of screen
      move_window_home();
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

static int check_key(const int inpkey)
{
  // value returned to indicate if more keys to be processed or terminate program
  int result = FRAMES;

  //printf("input %i\n", inpkey);
  
  switch(inpkey)
  {
    case 10:
      result = 0;
      break;
      
    case 27: // check escape sequence
      if (_kbhit()) check_esckey(getchar());
      else result = 0; // user pressed esc key to exit program
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
      set_key_down_update(zoom_window, 10.0f);
      break;
      
    case 'Z':
      set_key_down_update(zoom_window, -10.0f);
      break;
      
    case '<':
      set_key_down_update(change_angleVel, -10.0f);
      break;
      
    case '>':
      set_key_down_update(change_angleVel, 10.0f);
      break;
      
    case 'r':
      set_key_down_update(change_viewDist, -1.0f);
      break;
          
    case 'f':
      set_key_down_update(change_viewDist, 1.0f);
      break;
      
    case 'a':
      set_key_down_update(change_viewX, 1.0f);
      break;
          
    case 'd':
      set_key_down_update(change_viewX, -1.0f);
      break;
          
    case 'w':
      set_key_down_update(change_viewY, -1.0f);
      break;

    case 's':
      set_key_down_update(change_viewY, 1.0f);
      break;
          
          
    default: print_keyhelp();
  }
  
  // if falling behind in key processing then speed it up
  if (_kbhit()) result = 2; 
  return result;
}


static int detect_keypress(void)
{
  int active = FRAMES;
  int keyswaiting = _kbhit();
  
  if (keyswaiting) {
    //printf("keys waiting: %i\n", keyswaiting);
    active = check_key(getchar());
    // accelerate move rate
    move_rate_on();
  }
  else {
    // reset window move_rate to 1 pixel since no keys are being pressed
    move_rate_off();
  }
  
  return active;  
}