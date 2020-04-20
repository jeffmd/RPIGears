// font.h
#ifndef FONT_H_
  #define FONT_H_
  
  short Font_create(const char *filename);
  void Font_set_size(const short id, const int size);
  short Font_texture(const short id);
  void Font_set_active(const short id);
  void Font_active_bind(const int slot);
  short Font_active(void);
  float Glyph_u1(const int id, const char ch);
  float Glyph_v1(const int id, const char ch);
  float Glyph_u2(const int id, const char ch);
  float Glyph_v2(const int id, const char ch);
  int Glyph_advance(const int id, const char ch);
  int Glyph_width(const int id, const char ch);
  int Glyph_height(const int id, const char ch);
#endif
