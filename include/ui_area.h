// ui_area.h - user interface area header

#ifndef _UI_AREA_H_
  #define _UI_AREA_H_

  int UI_area_create(void);
  void UI_area_add(const int area_id, const int child_id);
  void UI_area_set_active(const int area_id);
  int UI_area_is_active(const int area_id);
  void UI_area_set_root(const int area_id);
  void UI_area_set_position(const int area_id, const int x, const int y);
  void UI_area_set_size(const int area_id, const int width, const int height);
  void UI_area_select_active(const int x, const int y);

#endif