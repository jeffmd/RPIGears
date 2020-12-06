// ui_area.h - user interface area header

#ifndef _UI_AREA_H_
  #define _UI_AREA_H_

  short UI_area_create(void);
  void UI_area_remove_parent(const short area_id);
  void UI_area_add(const short parent_id, const short child_id);
  void UI_area_set_active(const short area_id);
  int UI_area_is_active(const short area_id);
  void UI_area_set_root(const short area_id);
  void UI_area_set_offset(const short area_id, const short x, const short y);
  void UI_area_set_layout_position(const short area_id, const short x, const short y);
  void UI_area_offset(const short area_id, short offset_pos[2]);
  void UI_area_layout_position(const short area_id, short pos[2]);
  void UI_area_set_size(const short area_id, const short width, const short height);
  void UI_area_size(const short area_id, short size[2]);
  void UI_area_offset_size(const short area_id, short size[2]);
  short UI_area_offset_size_y(const short area_id);
  short UI_area_offset_size_x(const short area_id);
  void UI_area_set_hide(const short area_id, const int state);
  void UI_area_set_layout(const short area_id, const short layout);
  short UI_area_prev_sibling(const short area_id);
  short UI_area_next_sibling(const short area_id);
  void UI_area_set_locked(const short area_id);
  uint8_t UI_area_modid(const short area_id);
  short UI_area_pointer_x(void);
  short UI_area_pointer_y(void);
  short UI_area_rel_pointer_x(const short area_id);
  short UI_area_rel_pointer_y(const short area_id);

#endif