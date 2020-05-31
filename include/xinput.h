/*
* xinput.h
*/

#ifndef _XINPUT_H_
  #define _XINPUT_H_

  void Xinput_check_keys(XKeyEvent *event);
  void Xinput_pointer_move(const XMotionEvent* event);
  void Xinput_button_event(const XButtonEvent *event);

#endif
