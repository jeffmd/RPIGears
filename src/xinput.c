/*
*  xinput.c
*/

#include <stdint.h>
#include <stdio.h>
#include <X11/Xutil.h>

#include "id_plug.h"
#include "key_input.h"
#include "ui_area_action.h"

void Xinput_check_keys(XKeyEvent *event)
{
  //printf("keycode: %x, ", event->keycode);
  //printf( "Key: %x %c\n", (int)key, key );
  //printf( "Key state: %x\n", event->state );

  if (event->type == KeyPress) {
    KeySym keysym;
    char key;
    int keyout;

    XLookupString(event, &key, 1, &keysym, 0);
    //printf( "Key sym: %x key: %x\n", keysym, key );
    if (!key) {
      keyout = keysym;
    }
    else {
      keyout = key;
    }
    UI_area_key_change(STATE_KEY(event->state, keyout));
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

