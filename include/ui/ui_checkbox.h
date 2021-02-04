// ui_checkbox.h

#ifndef _UI_CHECKBOX_H_
  #define _UI_CHECKBOX_H_

  Handle_t UI_checkbox_create(const char *str, const Handle_t widget_handle);
  void UI_checkbox_connect_widget(const ID_t checkbox_id, const Handle_t widget_handle);
  void UI_checkbox_update_select(const ID_t id, const int val);
#endif
