/*
*  xinput.c
*/

#include <X11/Xutil.h>
#include <stdint.h>

#include "key_input.h"
#include "ui_area.h"

void xinput_check_keys(XKeyEvent *event)
{
  char key;

  XLookupString(event, &key, 1, 0, 0);
	//printf("keycode: %x, ", event->keycode);
	//printf( "Key: %x %c\n", (int)key, key );
        //printf( "Key state: %x\n", event.xkey.state );
	
  if (event->type == KeyPress)
    Key_input_action((event->state << 8) + key);
}

void xinput_pointer_move(const XMotionEvent* event)
{
  UI_area_select_active(event->x, event->y);
}

void xinput_button_event(const XButtonEvent *event)
{

}

