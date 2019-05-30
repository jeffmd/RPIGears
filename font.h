// font.h
#ifndef FONT_H_
  #define FONT_H_
  
  typedef struct Font Font;
  
  Font *font_create(const char *filename);
  void font_set_size(Font *font, int size);
  void font_set_active(Font *font);

#endif
