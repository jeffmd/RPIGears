// font.h
#ifndef FONT_H_
  #define FONT_H_
  
  typedef struct Glyph Glyph;

  int font_create(const char *filename);
  void font_set_size(const int id, const int size);
  int font_texture(const int id);
  void font_set_active(const int id);
  void font_active_bind(const int slot);
  int font_active(void);
  float glyph_u1(const int id, const char ch);
  float glyph_v1(const int id, const char ch);
  float glyph_u2(const int id, const char ch);
  float glyph_v2(const int id, const char ch);
  int glyph_advance(const int id, const char ch);
  int glyph_width(const int id, const char ch);
  int glyph_height(const int id, const char ch);
#endif
