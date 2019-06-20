/*
*  xinput.c
*/

#include <X11/Xutil.h>
#include <stdio.h>

#include "exit.h"
#include "key_input.h"

static void xinput_keypress(const char key)
{
  if (!check_key(key))
    exit_enable();
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

