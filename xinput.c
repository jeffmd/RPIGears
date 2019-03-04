/*
*  xinput.c
*/

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "tasks.h"
#include "key_input.h"

void x_process_keypress(KeySym key)
{
  if ((key == XK_Escape) | (key == XK_Return)) enable_exit();
  // conversion of XK values to 
  check_key(key);
}
