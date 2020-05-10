/*
*  xinput.c
*/

#include <X11/Xutil.h>
#include <stdint.h>

#include "key_input.h"
#include "ui_area.h"

void xinput_check_keys(XEvent *event)
{
  char key;

  XLookupString(&event->xkey, &key, 1, 0, 0);
	//printf("keycode: %x, ", event->xkey.keycode);
	//printf( "Key: %x %c\n", (int)key, key );
	
  if (event->type == KeyPress)
    Key_input_action(key);
}

void xinput_pointer_move(const XMotionEvent* event)
{
  UI_area_select_active(event->x, event->y);
}

void xinput_button_event(const XButtonEvent *event)
{

}

