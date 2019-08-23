/*
*  xinput.c
*/

#include <X11/Xutil.h>
#include <stdio.h>

#include "key_input.h"

void xinput_check_keys(XEvent *event)
{
  char key;

  XLookupString(&event->xkey, &key, 1, 0, 0);
	//printf("keycode: %x, ", event->xkey.keycode);
	//printf( "Key: %x %c\n", (int)key, key );
	
  if (event->type == KeyPress)
    key_input_action(key);
}
