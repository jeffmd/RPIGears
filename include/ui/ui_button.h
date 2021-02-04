// ui_button.h

#ifndef _UI_BUTTON_H_
  #define _UI_BUTTON_H_

  void UI_button_connect_widget(const short ui_slider_id, const Handle_t widget_handle);
  Handle_t UI_button_create(const char *str, const Handle_t widget_handle);

#endif