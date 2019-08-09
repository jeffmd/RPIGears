// font.h
#ifndef FONT_H_
  #define FONT_H_
  
  typedef struct Font Font;
  typedef struct Glyph Glyph;

  Font *font_create(const char *filename);
  void font_set_size(Font *font, int size);
  Glyph *font_glyph(Font *font, const char ch);
  int font_texture(Font *font);
  void font_set_active(Font *font);
  void font_active_bind(const int slot);
  Font *font_active(void);
  float glyph_u1(Glyph *glyph);
  float glyph_v1(Glyph *glyph);
  float glyph_u2(Glyph *glyph);
  float glyph_v2(Glyph *glyph);
  int glyph_advance(Glyph *glyph);
  int glyph_width(Glyph *glyph);
  int glyph_height(Glyph *glyph);
#endif
