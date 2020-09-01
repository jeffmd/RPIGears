// ui_checkbox.h

#ifndef _UI_CHECKBOX_H_
  #define _UI_CHECKBOX_H_

  int UI_checkbox_create(const char *str, const int handle);
  void UI_checkbox_connect_widget(const short checkbox_id, const int handle);
  void UI_checkbox_update_select(const short id, const int val);
#endif
