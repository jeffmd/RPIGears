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
  uint8_t ready;           // 1 if ready to draw
  GLfloat ProjMatrix[4];
  uint16_t index;          // current index into vertex buffer
  uint16_t count;          // mumber of characters in text vertex buffer
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
    GPU_uniformbuffer_add_uniform_4f(ubuff, "ProjMat", text->ProjMatrix);
    text->ProjMatrix[0] = 1.0/1280;
    text->ProjMatrix[1] = 1.0/1024;
    text->ProjMatrix[2] = -0.49f;
    text->ProjMatrix[3] = -0.49f;
    text->count = 0;
    text->index = 0;
  }
  
  if (!vformat) {
    vformat = GPU_vertex_format_create();
    GPU_vertex_format_add_attribute(vformat, "position_uv", 4, GL_HALF_FLOAT_OES);
  }

  GPUVertBuffer *vbuff = GPU_batch_vertex_buffer(text->batch);
  GPU_vertbuf_set_vertex_format(vbuff, vformat);
  GPU_vertbuf_set_add_count(vbuff, QUAD_SZE * MAX_CHAR_LENGTH);
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

  Glyph *glyph = font_glyph(text->font, ch);
  int advance = 0;
  
  if (glyph) {
    GPUVertBuffer *vbuff = GPU_batch_vertex_buffer(text->batch);
    
    const int dx = glyph_width(glyph);
    const int dy = glyph_height(glyph);
    const float u1 = glyph_u1(glyph);
    const float v1 = glyph_v1(glyph);
    const float u2 = glyph_u2(glyph);
    const float v2 = glyph_v2(glyph);

    if (ch !=' ') {
      advance = dx + 2;
    }
    else {
      advance = glyph_advance(glyph);
    }
    
    // build two triangles in vertex buffer
    VERTEX(x+dx, y, u2, v1);
    VERTEX(x, y+dy, u1, v2);
    VERTEX(x, y,    u1, v1);
    
    VERTEX(x+dx, y,    u2, v1);
    VERTEX(x+dx, y+dy, u2, v2);
    VERTEX(x, y+dy,    u1, v2);
    
    text->index++;
  }
    
  return advance;
}

static void text_update_draw_count(Text *text)
{
  if (text->index >= text->count) {
    text->count = text->index + 1;
    GPU_batch_set_vertices_draw_count(text->batch, text->count * QUAD_SZE);
  }
  
}

static void text_update_start(Text *text)
{
  GPUVertBuffer *vbuff = GPU_batch_vertex_buffer(text->batch);
  
  GPU_vertbuf_set_start(vbuff, text->index * QUAD_SZE);
}

void text_add(Text *text, int x, int y, const char *str)
{
  if (str) {
    Font *font = text_font(text);
    if (font) {
      
      text_update_start(text);
      
      for (; *str; str++) {
        x += add_quad_char(text, x, y, *str);
      }
      
      text_update_draw_count(text);
      text->ready = 1;
    }
  }
}

void text_set_start(Text *text, const int index)
{
  text->index = index;
}

int text_start(Text *text)
{
  return text->index;
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
