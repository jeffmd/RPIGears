// ui_checkbox.h

#ifndef _UI_CHECKBOX_H_
  #define _UI_CHECKBOX_H_

  int UI_checkbox_create(const char *str, const int handle);
  short UI_checkbox_connector(const short destination_class);
  void UI_checkbox_connect(const short checkbox_id, const int handle);
  void UI_checkbox_connect_select(const short connector_id, ActionFn action);
  void UI_checkbox_connect_update(const short connector_id, ActionFn action);
#endif
