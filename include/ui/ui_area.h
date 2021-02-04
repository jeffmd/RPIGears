// ui_area.h - user interface area header

#ifndef _UI_AREA_H_
  #define _UI_AREA_H_

  ID_t UI_area_create(void);
  void UI_area_remove_parent(const ID_t area_id);
  void UI_area_add(const ID_t parent_id, const ID_t child_id);
  void UI_area_set_active(const ID_t area_id);
  int UI_area_is_active(const ID_t area_id);
  void UI_area_set_root(const ID_t area_id);
  void UI_area_set_offset(const ID_t area_id, const short x, const short y);
  void UI_area_change_offset(const ID_t area_id, const short x, const short y);
  void UI_area_set_layout_position(const ID_t area_id, const short x, const short y);
  void UI_area_offset(const ID_t area_id, short offset_pos[2]);
  void UI_area_layout_position(const ID_t area_id, short pos[2]);
  void UI_area_set_size(const ID_t area_id, const short width, const short height);
  void UI_area_size(const ID_t area_id, short size[2]);
  void UI_area_offset_size(const ID_t area_id, short size[2]);
  short UI_area_offset_size_y(const ID_t area_id);
  short UI_area_offset_size_x(const ID_t area_id);
  void UI_area_set_hide(const ID_t area_id, const int state);
  void UI_area_set_layout(const ID_t area_id, const ID_t layout);
  ID_t UI_area_prev_sibling(const ID_t area_id);
  ID_t UI_area_next_sibling(const ID_t area_id);
  void UI_area_set_locked(const ID_t area_id);
  uint8_t UI_area_modid(const ID_t area_id);
  short UI_area_pointer_x(void);
  short UI_area_pointer_y(void);
  short UI_area_rel_pointer_x(const ID_t area_id);
  short UI_area_rel_pointer_y(const ID_t area_id);

#endif