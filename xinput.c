/*
*  xinput.c
*/

#include <X11/Xutil.h>
//#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>

#include "tasks.h"
#include "key_input.h"

static void xinput_keypress(const char key)
{
  //move_rate_on();
  if (!check_key(key))
    enable_exit();
}

void xinput_check_keys(XEvent *event)
{
  char key;

  XLookupString(&event->xkey, &key, 1, 0, 0);
	//printf("keycode: %x, ", event->xkey.keycode);
	//printf( "Key: %x %c\n", (int)key, key );
	
  if (event->type == KeyPress)
    xinput_keypress(key);
}

