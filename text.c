// text.c - build a text buffer that can be drawn in gles2

#include <stdio.h>

#include "gles3.h"

#include "gpu_texture.h"
#include "gpu_vertex_format.h"
#include "gpu_vertex_buffer.h"
#include "gpu_index_buffer.h"
#include "gpu_uniform_buffer.h"
#include "gpu_batch.h"
#include "shaders.h"
#include "static_array.h"
#include "font.h"

typedef struct {

  uint8_t active;           // zero if deleted
  GPUBatch *batch;
  Font *font;
  const char *str;
  uint8_t ready;
  float scale;
  
} Text;

enum {
  ATTR_POSITION,
};

#define TEXT_MAX_COUNT 5
#define QUAD_SZE 6
#define MAX_CHAR_LENGTH 100

static Text texts[TEXT_MAX_COUNT];
static Text *next_deleted_text = 0;

static GPUVertFormat *vformat = 0;

static inline Text *find_deleted_text(void)
{
  return ARRAY_FIND_DELETED(next_deleted_text, texts,
                            TEXT_MAX_COUNT, "text");
}

void text_init(Text *text)
{
  if (!text->batch) {
    text->batch = GPU_batch_create();
    GPUUniformBuffer *ubuff = GPU_batch_uniform_buffer(text->batch);
    GPU_uniformbuffer_add_uniform_1f(ubuff, "scale", text->scale);
    text->scale = 0.001f;
  }
  
  if (!vformat) {
    vformat = GPU_vertex_format_create();
    GPU_vertex_format_add_attribute(vformat, "position_uv", 4, GL_HALF_FLOAT_OES);
  }

  GPUVertBuffer *vbuff = GPU_batch_vertex_buffer(text->batch);
  GPU_vertbuf_set_vertex_format(vbuff, vformat);
  GPU_vertbuf_begin_update(vbuff, QUAD_SZE * MAX_CHAR_LENGTH);
  text->ready = 0;
}

Text *text_create(void)
{
  Text *text = find_deleted_text();

  text->active = 1;
  text_init(text);
  
  return text;
}

void text_delete(Text *text)
{
  text->active = 0;
  
  if (text < next_deleted_text)
    next_deleted_text = text; 
}

void text_set_font(Text *text, Font *font)
{
  text->font = font;
  text->ready = 0;
}

static Font *text_font(Text *text)
{
  if (!text->font) {
    text->font = font_active();    
  }
  
  return text->font;
}

static int add_quad_char(Text *text, const int x, const int y, const char ch)
{
#define VERTEX(x1, y1, u, v) GPU_vertbuf_add_4(vbuff, ATTR_POSITION, (x1), (y1), u, v)

  GPUVertBuffer *vbuff = GPU_batch_vertex_buffer(text->batch);
  Glyph *glyph = font_glyph(text->font, ch);
  int advance = 0;
  
  if (glyph) {
    if (ch !=' ') {
      const int dx = glyph_width(glyph);
      const int dy = glyph_height(glyph);
      const float u1 = glyph_u1(glyph);
      const float v1 = glyph_v1(glyph);
      const float u2 = glyph_u2(glyph);
      const float v2 = glyph_v2(glyph);

      advance = dx + 2;
      // build two triangles in vertex buffer
      VERTEX(x+dx, y, u2, v1);
      VERTEX(x, y+dy, u1, v2);
      VERTEX(x, y,    u1, v1);
      
      VERTEX(x+dx, y,    u2, v1);
      VERTEX(x+dx, y+dy, u2, v2);
      VERTEX(x, y+dy,    u1, v2);
    }
    else {
      advance = glyph_advance(glyph);
    }
  }
    
  return advance;
}

void text_add(Text *text, int x, int y, const char *str)
{
  if (str) {
    Font *font = text_font(text);
    if (font) {
      int count = 0;
      
      for (; *str; str++) {
        x += add_quad_char(text, x, y, *str);
        count++;
      }
      
      text->ready = 1;
      GPU_batch_set_vertices_draw_count(text->batch, count * QUAD_SZE);
    }
  }
}

void text_set_scale(Text *text, float scale)
{
  text->scale = scale;
}

void text_draw(Text *text)
{
  if (text->ready) {
    GPU_texture_bind(font_texture(text->font), 0);
    shaders_bind_test_quad_shader();
    GPU_uniformbuffer_activate(0);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GPU_batch_draw(text->batch, GL_TRIANGLES, 1);  
    //glDisable(GL_BLEND);
  }    
}
