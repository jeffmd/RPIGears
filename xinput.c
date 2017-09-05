/*
*  xinput.c
*/

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "tasks.h"

void x_process_keypress(KeySym key)
{
  if ((key == XK_Escape) | (key == XK_Return)) enable_exit();
}
