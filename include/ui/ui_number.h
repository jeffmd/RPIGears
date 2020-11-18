// ui_number.h

#ifndef _UI_NUMBER_H_
  #define _UI_NUMBER_H_

  int UI_number_create(const char *str, const int handle);
  void UI_number_connect_widget(const short checkbox_id, const int handle);
  void UI_number_update_float(const short id, const float val);
  void UI_number_update_int(const short id, const int val);
  float UI_number_float_change(const short number_id);
  int UI_number_int_change(const short number_id);
  void UI_number_set_default_float_change(const short number_id, const float val);
  void UI_number_set_default_int_change(const short number_id, const int val);
  void UI_number_set_edit(const short number_id, const int state);

#endif