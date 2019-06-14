// font.c

#include <stdio.h>
#include <stdint.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "gles3.h"
#include "gpu_texture.h"
#include "static_array.h"

#define CHAR_START 32
#define CHAR_END 127
#define CHAR_SET CHAR_END - CHAR_START 

typedef struct Glyph {
  float u1;
  float v1;
  float u2;
  float v2;
  int advance;
  int width;
  int height;
  
} Glyph;

typedef struct Font {
  uint8_t active;
  uint8_t size;
  uint8_t ready;
  
  Glyph glyphs[CHAR_SET];
  GPUTexture *texture;
  const char *filename;
  
} Font;

#define FONT_MAX_COUNT 10

static Font fonts[FONT_MAX_COUNT];
static Font *next_deleted_font = 0;

static Font *active_font = 0;

#define TEXTURE_GLYPH_SLOTS 8
#define DEFAULT_GLYPH_HEIGHT 30
#define GLYPHSPC 2
#define TEXTURE_SIZE TEXTURE_GLYPH_SLOTS * (font->size + GLYPHSPC)

static FT_Library ft_lib;
static FT_Face face;

static const char default_path[] = "/usr/share/fonts/truetype/";
static char path[255];

static void init_texture(Font * font)
{
  if (!font->texture) {
    font->texture = GPU_texture_create(TEXTURE_SIZE,
      TEXTURE_SIZE, GPU_R8, 0);
  }
}

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
    
    init_texture(font);
    
    //int roww = 0;
		int rowh = 0;
    int ox = GLYPHSPC;
		int oy = DEFAULT_GLYPH_HEIGHT + GLYPHSPC;
    const float tex_width = GPU_texture_width(font->texture);
    const float tex_height = GPU_texture_height(font->texture);

    for (int i = CHAR_START; i < CHAR_END; i++) {
      if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
        printf("Loading character %c failed!\n", i);
        continue;
      }

      if (glyph->bitmap.rows > rowh)
        rowh = glyph->bitmap.rows;

      if ((ox + glyph->bitmap.width + GLYPHSPC) >= TEXTURE_SIZE) {
        oy += rowh + GLYPHSPC;
        rowh = 0;
        ox = GLYPHSPC;
      }

      GPU_texture_sub_image(font->texture, ox, oy - glyph->bitmap_top, glyph->bitmap.width, glyph->bitmap.rows, glyph->bitmap.buffer);
      
      Glyph *glyphc = &font->glyphs[i - CHAR_START];
      
      glyphc->advance = glyph->advance.x >> 6;
      glyphc->height = glyph->bitmap.rows;
      glyphc->width = glyph->bitmap.width;
      glyphc->u1 = (float)ox / tex_width;
      glyphc->v1 = (float)(oy - glyph->bitmap_top + glyphc->height) / tex_height;
      glyphc->u2 = (float)(ox + glyphc->width) / tex_width;
      glyphc->v2 = (float)(oy - glyph->bitmap_top) / tex_height;
      
      printf("glyph: %c, width: %i height: %i left: %i. top: %i, advance: %i\n", i,
             glyph->bitmap.width, glyph->bitmap.rows, glyph->bitmap_left, glyph->bitmap_top, glyphc->advance);

      ox += glyphc->width + GLYPHSPC;
    }
    
    FT_Done_Face(face);
    face = 0;
    
    GPU_texture_mipmap(font->texture);	
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

Font *font_active(void)
{
  return active_font;
}

void font_active_bind(const int slot)
{
  if (active_font && (active_font->ready)) {
    GPU_texture_bind(active_font->texture, slot);
  }
}

GPUTexture *font_texture(Font *font)
{
  if (font)
    return font->texture;
  else
    return 0;
}

Glyph *font_glyph(Font *font, const char ch)
{
  if (font)
    return &font->glyphs[ch - CHAR_START];
  else
    return 0;
}

float glyph_u1(Glyph *glyph)
{
  return glyph->u1;
}

float glyph_v1(Glyph *glyph)
{
  return glyph->v1;
}

float glyph_u2(Glyph *glyph)
{
  return glyph->u2;
}

float glyph_v2(Glyph *glyph)
{
  return glyph->v2;
}

int glyph_advance(Glyph *glyph)
{
  return glyph->advance;
}

int glyph_width(Glyph *glyph)
{
  return glyph->width;
}

int glyph_height(Glyph *glyph)
{
  return glyph->height;
}

