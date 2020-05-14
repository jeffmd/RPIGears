/*
* xinput.h
*/

#ifndef _XINPUT_H_
  #define _XINPUT_H_

  void xinput_check_keys(XKeyEvent *event);
  void xinput_pointer_move(const XMotionEvent* event);
  void xinput_button_event(const XButtonEvent *event);

#endif
