// font.h
#ifndef FONT_H_
  #define FONT_H_
  
  ID_t Font_create(const char *filename);
  void Font_set_size(const ID_t id, const int size);
  ID_t Font_texture(const ID_t id);
  void Font_set_active(const ID_t id);
  void Font_active_bind(const int slot);
  ID_t Font_active(void);
  float Glyph_u1(const ID_t id, const char ch);
  float Glyph_v1(const ID_t id, const char ch);
  float Glyph_u2(const ID_t id, const char ch);
  float Glyph_v2(const ID_t id, const char ch);
  int Glyph_advance(const ID_t id, const char ch);
  int Glyph_width(const ID_t id, const char ch);
  int Glyph_height(const ID_t id, const char ch);
#endif
