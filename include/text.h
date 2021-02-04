// text.h

#ifndef TEXT_H_
  #define TEXT_H_
    
  ID_t Text_create(void);
  void Text_set_font(const ID_t id, const ID_t font);
  void Text_set_offset(const ID_t id, const int width, const int height);
  void Text_add(const ID_t id, const int x, const int y, const char *str);
  void Text_set_index(const ID_t id, const int index);
  short Text_index(const ID_t id);
  void Text_draw(const ID_t id);
  void Text_extent(const ID_t id, int extent[2]);
  short Text_pos_x(const ID_t id);
  short Text_pos_y(const ID_t id);
  void Text_sync_pos(const ID_t id);
  short Text_cursor_offset_x(const ID_t id, const int index);
  short Text_cursor_find_index(const ID_t id, const short offset_x);

#endif
