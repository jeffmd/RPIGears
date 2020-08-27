// text.h

#ifndef TEXT_H_
  #define TEXT_H_
    
  short Text_create(void);
  void Text_set_font(const short id, const short font);
  void Text_set_offset(const short id, const int width, const int height);
  void Text_add(const short id, const int x, const int y, const char *str);
  void Text_set_index(const short id, const int index);
  short Text_index(const short id);
  void Text_draw(const short id);
  void Text_extent(const short id, int extent[2]);
  short Text_pos_x(const short id);
  short Text_pos_y(const short id);

#endif
