// ui_slider.h

#ifndef _UI_SLIDER_H_
  #define _UI_SLIDER_H_

  float UI_slider_travel_percent(const ID_t ui_slider_id);
  void UI_slider_set_travel_percent(const ID_t ui_slider_id, const float percent);
  void UI_slider_set_full_travel(const ID_t ui_slider_id, const short max_travel);
  void UI_slider_set_travel_coverage(const ID_t ui_slider_id, const float travel_coverage);
  void UI_slider_connect_widget(const ID_t ui_slider_id, const Plug_t widget_plug);
  Plug_t UI_slider_create(const Plug_t widget_plug);

#endif