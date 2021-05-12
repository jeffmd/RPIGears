// ui_button.h

#ifndef _UI_BUTTON_H_
  #define _UI_BUTTON_H_

  int UI_button_pressed(const ID_t ui_button_id);
  void UI_button_connect_widget(const ID_t ui_slider_id, const Plug_t widget_plug);
  Plug_t UI_button_create(const char *str, const Plug_t widget_plug);

#endif