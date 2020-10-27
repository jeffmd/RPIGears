/*
*  xinput.c
*/

#include <stdint.h>
#include <stdio.h>
#include <X11/Xutil.h>

#include "key_input.h"
#include "ui_area_action.h"

void Xinput_check_keys(XKeyEvent *event)
{
  //printf("keycode: %x, ", event->keycode);
  //printf( "Key: %x %c\n", (int)key, key );
  //printf( "Key state: %x\n", event->state );
  if (event->type == KeyPress) {
    char key;

    XLookupString(event, &key, 1, 0, 0);
    UI_area_key_change(STATE_KEY(event->state, key));
  }
}

void Xinput_pointer_move(const XMotionEvent* event)
{
  UI_area_select_active(STATE_KEY(event->state, 0), event->x, event->y);
}

void Xinput_button_event(const XButtonEvent *event)
{
  if (event->type == ButtonPress) {
    UI_area_key_change(STATE_KEY(event->state, KEY_BUTTON(event->button)));
  }
  else {
    UI_area_key_change(KEY_BUTTON_RELEASE(event->button));
  }
}

