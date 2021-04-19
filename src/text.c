// text.c - build a text buffer that can be drawn in gles2

#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"

#include "id_plug.h"
#include "static_array.h"
#include "gpu_texture.h"
#include "gpu_vertex_format.h"
#include "gpu_vertex_buffer.h"
#include "gpu_index_buffer.h"
#include "gpu_uniform_buffer.h"
#include "gpu_batch.h"
#include "gpu_shader.h"
#include "shaders.h"
#include "font.h"

typedef struct {

  uint8_t active;          // zero if deleted
  uint8_t ready:1;         // 1 if ready to draw
  ID_t font;
  short extent[2];         // text x and y extent
  short pos_x;             // last x position of add character
  short pos_y;             // last y position of add character
  short offset_x;          // text offset x 
  short start;             // start index in vertex buffer
  short index;             // index for next char
  short count;             // mumber of characters in text vertex buffer
  GLfloat ProjMatrix[4];   // scale and offset
  GLfloat alimit;          // alpha limit
} Text;

enum {
  ATTR_POSITION,
};

#define TEXT_MAX_COUNT 50
#define QUAD_SZE 6
#define MAX_CHAR_LENGTH 100

static Text texts[TEXT_MAX_COUNT];
static ID_t next_deleted_text;

static ID_t vformat;
static ID_t batch;
static short index;          // current master index into vertex buffer
static GLfloat ProjMatrix[4];   // scale and offset
static GLfloat alimit;


static inline ID_t find_deleted_text_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_text, texts, TEXT_MAX_COUNT, Text,"text");
}

static Text *get_text(ID_t id)
{
  if ((id < 0) | (id >= TEXT_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Text id, using default id: 0\n");
  }
    
  return texts + id;
}

static ID_t text_vformat(void)
{
  if (!vformat) {
    vformat = GPU_vertex_format_create();
    GPU_vertex_format_add_attribute(vformat, "position_uv", 4, GL_HALF_FLOAT_OES);
  }
  return vformat;
}

static void text_default_settings(Text *text)
{
    text->ProjMatrix[0] = 1.0f/1280.0f;
    text->ProjMatrix[1] = 1.0f/1024.0f;
    text->alimit = 0.5f;
}

static ID_t get_batch(void)
{
  if (!batch) {
    batch = GPU_batch_create();
    const ID_t ubuff = GPU_batch_uniform_buffer(batch);
    GPU_uniformbuffer_add_4f(ubuff, "ProjMat", ProjMatrix);
    GPU_uniformbuffer_add_1f(ubuff, "alimit", alimit);

    const ID_t vbuff = GPU_batch_vertex_buffer(batch);
    GPU_vertbuf_set_vertex_format(vbuff, text_vformat());
    GPU_vertbuf_set_add_count(vbuff, QUAD_SZE * MAX_CHAR_LENGTH);
  }

  return batch;
}

static void text_init(Text *text)
{
  text_default_settings(text);

  text->start = index;
  text->index = index;
  text->count = 0;
  text->ready = 0;
  
}

ID_t Text_create(void)
{
  const ID_t id = find_deleted_text_id();
  Text * const text = get_text(id);

  text->active = 1;
  text_init(text);
  
  return id;
}

void Text_delete(const ID_t id)
{
  Text * const text = get_text(id);
  text->active = 0;
  
  if (id < next_deleted_text)
    next_deleted_text = id; 
}

void Text_set_font(const ID_t id, const ID_t font)
{
  Text * const text = get_text(id);
  text->font = font;
  text->ready = 0;
}

static int text_font(Text *text)
{
  if (!text->font) {
    text->font = Font_active();    
  }
  
  return text->font;
}

static void update_extent(Text *text, int x, int y)
{
  if (x > text->extent[0])
    text->extent[0] = x;

  if (y > text->extent[1])
    text->extent[1] = y;
}

static void increment_index(Text *text)
{
  text->index++;
  if (text->index > index) {
    index = text->index;
  }
}

static int add_quad_char(Text *text, const int x, const int y, const char ch)
{
#define VERTEX(x1, y1, u, v) GPU_vertbuf_add_4(vbuff, ATTR_POSITION, (x1), (y1), u, v)

  int advance = 0;
  
  const ID_t vbuff = GPU_batch_vertex_buffer(get_batch());
  const ID_t font = text->font;
  
  const int dx = Glyph_width(font, ch);
  const int dy = Glyph_height(font, ch);

  const float x1 = x;
  const float y1 = y;

  const float x2 = x + dx;
  const float y2 = y + dy;

  const float u1 = Glyph_u1(font, ch);
  const float v1 = Glyph_v1(font, ch);
  const float u2 = Glyph_u2(font, ch);
  const float v2 = Glyph_v2(font, ch);

  if (ch !=' ') {
    advance = dx + 2;
  }
  else {
    advance = Glyph_advance(font, ch);
  }
  
  // build two triangles in vertex buffer

  VERTEX(x2, y1, u2, v1);
  VERTEX(x1, y2, u1, v2);
  VERTEX(x1, y1,  u1, v1);
  
  VERTEX(x2, y1,  u2, v1);
  VERTEX(x2, y2, u2, v2);
  VERTEX(x1, y2,  u1, v2);
  
  increment_index(text);
  update_extent(text, x+dx, y+dy);

  return advance;
}

static void text_update_draw_count(Text *text)
{
  const uint16_t new_count = text->index - text->start;

  if (new_count > text->count) {
    text->count = new_count;
  }
}

static void text_update_start(Text *text)
{
  const ID_t vbuff = GPU_batch_vertex_buffer(get_batch());
  
  GPU_vertbuf_set_index(vbuff, text->index * QUAD_SZE);
}

void Text_add(const ID_t id, int x, int y, const char *str)
{
  if (str) {
    Text * const text = get_text(id);
    
    if (text_font(text)) {
      text_update_start(text);
      
      for (; *str; str++) {
        x += add_quad_char(text, x, y, *str);
      }
      
      text_update_draw_count(text);
      text->pos_x = x;
      text->pos_y = y;
      text->ready = 1;
    }
  }
}

static void set_index(Text * const text, const short index)
{
  text->index = text->start + index;
}
 
void Text_set_index(const ID_t id, const short index)
{
  set_index(get_text(id), index);
}

static short get_index(Text * const text)
{
  return (text->index - text->start);
}
 
short Text_index(const ID_t id)
{
  return get_index(get_text(id));
}

void Text_set_offset(const ID_t id, const int width, const int height)
{
  Text * const text = get_text(id);
  const float s0 = width;
  const float m0 = text->ProjMatrix[0];
  const float s1 = height;
  const float m1 = text->ProjMatrix[1];

  const float s2 = -s0 * m0;
  const float s3 = -s1 * m1;

  text->ProjMatrix[2] = s2;
  text->ProjMatrix[3] = s3;
  text->offset_x = width;
}

static void update_uniforms(Text *text)
{
  const GLfloat val1 = text->ProjMatrix[0];
  const GLfloat val2 = text->ProjMatrix[1];
  const GLfloat val3 = text->ProjMatrix[2];
  const GLfloat val4 = text->ProjMatrix[3];
  const GLfloat val5 = text->alimit;

  ProjMatrix[0] = val1;
  ProjMatrix[1] = val2;
  ProjMatrix[2] = val3;
  ProjMatrix[3] = val4;

  alimit = val5;
}

void Text_draw(const ID_t id)
{
  Text * const text = get_text(id);
  if (text->ready) {
    update_uniforms(text);
    GPU_texture_bind(Font_texture(text->font), 0);
    GPU_shader_bind(Shaders_test_quad());
    GPU_uniformbuffer_activate(0);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // batch has been setup so safe to use var
    GPU_batch_set_vertices_draw_count(batch, text->count * QUAD_SZE);
    GPU_batch_set_draw_start(batch, text->start * QUAD_SZE);
    GPU_batch_draw(batch, GL_TRIANGLES, 1);  
    //glDisable(GL_BLEND);
  }    
}

void Text_extent(const ID_t id, int extent[2])
{
  Text * const text = get_text(id);
  extent[0] = text->extent[0] / 2 + 4;
  extent[1] = text->extent[1] / 2 + 4;
}

short Text_pos_x(const ID_t id)
{
  return get_text(id)->pos_x;
}

short Text_pos_y(const ID_t id)
{
  return get_text(id)->pos_y;
}

static void sync_pos(Text * const text)
{
  GLfloat pos[4];
  const ID_t vbuff = GPU_batch_vertex_buffer(get_batch());

  GPU_vertbuf_set_index(vbuff, text->index * QUAD_SZE + 1);
  GPU_vertbuf_read_data(vbuff, ATTR_POSITION, pos);
  text->pos_x = pos[0];
  text->pos_y = pos[1];
}

void Text_sync_pos(const ID_t id)
{
  sync_pos(get_text(id));
}

short Text_cursor_offset_x(const ID_t id, const int index)
{
  Text * const text = get_text(id);
  set_index(text, index);
  sync_pos(text);

  return text->pos_x - text->offset_x;
}

short Text_cursor_find_index(const ID_t id, const short offset_x)
{
  short old_delta = 1000;
  short delta;
  int found = 0;
  Text * const text = get_text(id);
  const short rel_offset = (offset_x * 2) + text->offset_x;
  const int maxcount = text->count - 1;

  set_index(text, 0);

  while (!found) {
    sync_pos(text);
    delta = abs(rel_offset - text->pos_x);

    if ((delta <= old_delta) && (get_index(text) < maxcount)) {
      old_delta = delta;
      text->index++;
    }
    else {
      found = 1;
    }
  }
  
  if (get_index(text) > 0) {
    text->index--;
  }

  return get_index(text);
}

