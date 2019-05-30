// font.c

#include <stdio.h>
#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "gles3.h"
#include "gpu_texture.h"
#include "static_array.h"


typedef struct Font {
  uint8_t active;
  uint8_t size;
  uint8_t ready;
  GPUTexture *texture;
  const char *filename;
  
} Font;

#define FONT_MAX_COUNT 10

static Font fonts[FONT_MAX_COUNT];
static Font *next_deleted_font = 0;

static Font *active_font = 0;

#define DEFAULT_GLYPH_HEIGHT 12

static FT_Library ft_lib;
static FT_Face face;

static const char default_path[] = "/usr/share/fonts/truetype/";
static char path[255];

static void transfer_glyphs(Font *font)
{
  strcpy(path, default_path);
  strcat(path, font->filename);

  if (FT_New_Face(ft_lib, path, 0, &face)) {
    printf("Could not open font %s\n", font->filename);
  }

  if (face) {
    FT_Set_Pixel_Sizes(face, 0, font->size);
    FT_GlyphSlot glyph = face->glyph;
    for (int i = 32; i < 128; i++) {
      if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
        printf("Loading character %c failed!\n", i);
        continue;
      }

      //if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
      //  oy += rowh;
      //  rowh = 0;
      //  ox = 0;
      //}

      //glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
      
      //c[i].ax = glyph->advance.x >> 6;
      //c[i].ay = g->advance.y >> 6;

      //c[i].bw = g->bitmap.width;
      printf("glyph: %c, width: %i height: %i left: %i. top: %i\n", i,
             glyph->bitmap.width, glyph->bitmap.rows, glyph->bitmap_left, glyph->bitmap_top);
      //c[i].bh = g->bitmap.rows;

      //c[i].bl = g->bitmap_left;
      //c[i].bt = g->bitmap_top;
    
    }

    FT_Done_Face(face);
    face = 0;
    
    font->ready = 1;
  }
}

static void build_glyphs(Font *font)
{
  if (!ft_lib) {
    if (FT_Init_FreeType(&ft_lib)) {
      printf("Could not init freetype library\n");
    }
  }
  
  if (ft_lib) {
    transfer_glyphs(font);
    FT_Done_FreeType(ft_lib);
    ft_lib = 0;
  }  
}

static inline Font *find_deleted_font(void)
{
  return ARRAY_FIND_DELETED(next_deleted_font, fonts, FONT_MAX_COUNT, "font");
}

static Font *find_font(const char *filename)
{
  for (int idx = 0; idx < FONT_MAX_COUNT; idx++) {
    Font *font = &fonts[idx];
    
    if (font->filename) {
      if (strcmp(font->filename, filename) == 0) {
          return font;
      }
    }
  }
  
  return 0;
}

static void font_init(Font *font)
{
  if (!font->size) {
    font->size = DEFAULT_GLYPH_HEIGHT;
  }
  
  font->ready = 0; 
}

Font *font_create(const char *filename)
{
  Font *font = find_font(filename);
  
  if (!font) {
    font = find_deleted_font();
    font->filename = filename;
    font_init(font);
  }
  
  font->active = 1;
  
  return font;
}

void font_set_size(Font *font, int size)
{
  if (font->size != size) {
    font->size = size;
    font->ready = 0;
  }
}

void font_set_active(Font *font)
{
  if (!font->ready) {
    build_glyphs(font);
  }
  
  if (font->ready) {
    active_font = font;
  }
}
