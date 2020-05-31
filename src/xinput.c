/*
*  xinput.c
*/

#include <stdint.h>
#include <stdio.h>
#include <X11/Xutil.h>

#include "key_input.h"
#include "ui_area_action.h"

void xinput_check_keys(XKeyEvent *event)
{
  char key;

  XLookupString(event, &key, 1, 0, 0);
	//printf("keycode: %x, ", event->keycode);
	//printf( "Key: %x %c\n", (int)key, key );
        //printf( "Key state: %x\n", event->state );
	
  if (event->type == KeyPress)
    UI_area_key_change(STATE_KEY(event->state, key));
}

void xinput_pointer_move(const XMotionEvent* event)
{
  UI_area_select_active(event->x, event->y);
}

void xinput_button_event(const XButtonEvent *event)
{
  if (event->type == ButtonPress)
    UI_area_key_change(STATE_KEY(event->state, KEY_BUTTON(event->button)));
  printf( "Key state: %x\n", event->state );
}

