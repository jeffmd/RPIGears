// ui_area.h - user interface area header

#ifndef _UI_AREA_H_
  #define _UI_AREA_H_

  short UI_area_create(void);
  void UI_area_remove_parent(const short area_id);
  void UI_area_add(const short parent_id, const short child_id);
  void UI_area_set_active(const short area_id);
  int UI_area_is_active(const short area_id);
  void UI_area_set_root(const short area_id);
  void UI_area_set_position(const short area_id, const int x, const int y);
  void UI_area_set_size(const short area_id, const int width, const int height);
  void UI_area_size(const short area_id, int size[2]);
  void UI_area_set_hide(const short area_id, const int state);
  void UI_area_set_locked(const short area_id);
  uint8_t UI_area_modid(const short area_id);
  short UI_area_pointer_x(void);
  short UI_area_pointer_y(void);

#endif