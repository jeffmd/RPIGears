// font.h
#ifndef FONT_H_
  #define FONT_H_
  
  typedef struct Glyph Glyph;

  int font_create(const char *filename);
  void font_set_size(int id, int size);
  int font_texture(int id);
  void font_set_active(int id);
  void font_active_bind(const int slot);
  int font_active(void);
  float glyph_u1(int id, const char ch);
  float glyph_v1(int id, const char ch);
  float glyph_u2(int id, const char ch);
  float glyph_v2(int id, const char ch);
  int glyph_advance(int id, const char ch);
  int glyph_width(int id, const char ch);
  int glyph_height(int id, const char ch);
#endif
