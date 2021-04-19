// ui_number.h

#ifndef _UI_NUMBER_H_
  #define _UI_NUMBER_H_

  Plug_t UI_number_create(const char *str, const Plug_t widget_plug);
  void UI_number_connect_widget(const ID_t checkbox_id, const Plug_t widget_plug);
  void UI_number_update_float(const ID_t id, const float val);
  void UI_number_update_int(const ID_t id, const int val);
  float UI_number_float_change(const ID_t number_id);
  int UI_number_int_change(const ID_t number_id);
  float UI_number_float_new_val(const ID_t number_id);
  int UI_number_int_new_val(const ID_t number_id);
  void UI_number_set_default_float_change(const ID_t number_id, const float val);
  void UI_number_set_default_int_change(const ID_t number_id, const int val);
  void UI_number_set_edit(const ID_t number_id, const int state);

#endif