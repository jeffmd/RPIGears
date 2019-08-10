// test_quad.c
// render a texture to a quad

#include <stdlib.h>
#include <stdio.h>

#include "gles3.h"

#include "gpu_texture.h"
#include "gpu_vertex_format.h"
#include "gpu_vertex_buffer.h"
#include "gpu_index_buffer.h"
#include "gpu_uniform_buffer.h"
#include "gpu_batch.h"
#include "shaders.h"
#include "camera.h"
#include "key_input.h"

enum {
  ATTR_POSITION,
};

typedef struct {
  int texture;
  int batch;
  uint8_t toggle;
  GLfloat ProjMatrix[4];
} TQuad;

static TQuad quad;
static int vformat = 0;

static void test_quad_toggle(void)
{
  quad.toggle = !quad.toggle;
}

static void test_quad_delete(void)
{
  if (quad.batch) {
    GPU_batch_delete(quad.batch, 1);
    quad.batch = 0;
  }
  printf("test quad has shut down\n"); 
}

void test_quad(void)
{
  if (!quad.batch) {
    quad.batch = GPU_batch_create();
    const int ubuff = GPU_batch_uniform_buffer(quad.batch);
    GPU_uniformbuffer_add_4f(ubuff, "ProjMat", quad.ProjMatrix);
    quad.ProjMatrix[0] = 0.12f;
    quad.ProjMatrix[1] = 0.12f;
    quad.ProjMatrix[2] = 0.0f;
    quad.ProjMatrix[3] = 0.0f;
    
  }
  if (!vformat) {
    vformat = GPU_vertex_format_create();
    GPU_vertex_format_add_attribute(vformat, "position_uv", 4, GL_HALF_FLOAT_OES);
  }

  const int vbuff = GPU_batch_vertex_buffer(quad.batch);
  GPU_batch_set_vertices_draw_count(quad.batch, 6);
  GPU_vertbuf_set_vertex_format(vbuff, vformat);
  GPU_vertbuf_set_add_count(vbuff, 6);
  
#define VERTEX(x, y) (GPU_vertbuf_add_4(vbuff, ATTR_POSITION, x, y, ((x) + 1)*0.5, (-(y) + 1)*0.5))
#define QUAD(x, y, dx, dy) VERTEX(x+dx, y); VERTEX(x, y+dy); VERTEX(x, y);  VERTEX(x+dx, y); VERTEX(x+dx, y+dy); VERTEX(x, y+dy);
  
  QUAD(-1, -1, 2, 2)
  
  key_add_action('T', test_quad_toggle, "toggle test quad visibility");
  atexit(test_quad_delete);
  
}

void test_quad_draw(void)
{
  if (quad.toggle) {
    if (quad.texture)
      GPU_texture_bind(quad.texture, 0);
    shaders_bind_test_quad_shader();
    GPU_uniformbuffer_activate(0);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GPU_batch_draw(quad.batch, GL_TRIANGLES, 1);  
    //glDisable(GL_BLEND);
  }
}

void test_quad_set_texture(const int id)
{
  quad.texture = id;
}
