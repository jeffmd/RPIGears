// test_quad.c
// render a texture to a quad

#include "gles3.h"

#include "gpu_texture.h"
#include "gpu_vertex_format.h"
#include "gpu_vertex_buffer.h"
#include "gpu_index_buffer.h"
#include "gpu_uniform_buffer.h"
#include "gpu_batch.h"
#include "shaders.h"
#include "font.h"

enum {
  ATTR_POSITION,
  ATTR_UV,
};

typedef struct {
  GPUTexture *texture;
  GPUBatch *batch;
  uint8_t toggle; 
} TQuad;

static TQuad quad;
static GPUVertFormat *vformat = 0;

void test_quad(void)
{
  if (!quad.batch)
    quad.batch = GPU_batch_create();
  
  if (!vformat) {
    vformat = GPU_vertex_format_create();
    GPU_vertex_format_add_attribute(vformat, "position_uv", 4, GL_HALF_FLOAT_OES);
  }

  GPUVertBuffer *vbuff = GPU_batch_vertex_buffer(quad.batch);
  GPU_batch_set_vertices_draw_count(quad.batch, 6);
  GPU_vertbuf_set_vertex_format(vbuff, vformat);
  GPU_vertbuf_begin_update(vbuff, 6);
  
#define SZE 0.1
#define ISZE 0.5/SZE
#define VERTEX(x,y) (GPU_vertbuf_add_4(vbuff, ATTR_POSITION, x, y, (x + SZE)*ISZE, (-y + SZE)*ISZE))

  VERTEX(SZE, -SZE);
  VERTEX(-SZE, SZE);
  VERTEX(-SZE, -SZE);

  VERTEX(SZE, -SZE);
  VERTEX(SZE, SZE);
  VERTEX(-SZE, SZE);
  
}

void test_quad_draw(void)
{
  if (quad.toggle) {
    font_active_bind(0);
    shaders_bind_test_quad_shader();
    GPU_uniformbuffer_activate(0);
    GPU_batch_draw(quad.batch, GL_TRIANGLES, 1);  
  }
}

void test_quad_delete(void)
{
  if (quad.batch) {
    GPU_batch_delete(quad.batch, 1);
    quad.batch = 0;
  }
}

void test_quad_toggle(void)
{
  quad.toggle = !quad.toggle;
}
