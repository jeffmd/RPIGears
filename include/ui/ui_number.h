// ui_number.h

#ifndef _UI_NUMBER_H_
  #define _UI_NUMBER_H_
  int UI_number_create(const char *str, const int handle);
  short UI_number_connector(const short destination_class);
  void UI_number_connect(const short checkbox_id, const int handle);
  void UI_number_connect_select(const short connector_id, ActionFn action);
  void UI_number_connect_update(const short connector_id, ActionFn action);
  void UI_number_update_float(const short id, const float val);
  void UI_number_update_int(const short id, const int val);

#endif