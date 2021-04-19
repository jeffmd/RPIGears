// gpu_quad.c
// render a texture to a quad

#include <stdlib.h>
#include <stdio.h>

#include "gles3.h"

#include "id_plug.h"
#include "static_array.h"
#include "gpu_texture.h"
#include "gpu_vertex_format.h"
#include "gpu_vertex_buffer.h"
#include "gpu_uniform_buffer.h"
#include "gpu_batch.h"
#include "gpu_shader.h"

enum {
  ATTR_POSITION,
};

typedef struct {
  uint8_t active;
  ID_t texture;
  ID_t batch;
  ID_t shader;
  GLfloat ProjMatrix[4];
  GLfloat alimit;
} GPUQuad;

#define QUAD_MAX_COUNT 10

static short vformat = 0;

static GPUQuad quads[QUAD_MAX_COUNT];
static ID_t next_deleted_quad;

static inline ID_t find_deleted_quad_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_quad, quads, QUAD_MAX_COUNT, GPUQuad, "quad");
}

static GPUQuad *get_quad(ID_t id)
{
  if ((id < 0) | (id >= QUAD_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Quad id, using default id: 0\n");
  }
    
  return quads + id;
}

static void gpu_quad_delete_batch(GPUQuad *quad)
{
  if (quad->batch) {
    GPU_batch_delete(quad->batch, 1);
    quad->batch = 0;
  }
}

static void gpu_quad_init(GPUQuad *quad)
{
  gpu_quad_delete_batch(quad);

  quad->ProjMatrix[0] = 0.12f;
  quad->ProjMatrix[1] = 0.12f;
  quad->ProjMatrix[2] = 0.0f;
  quad->ProjMatrix[3] = 0.0f;
  quad->alimit = 0.5f;
}

void GPU_quad_delete(const ID_t id)
{
  GPUQuad *const quad = get_quad(id);

  quad->active = 0;
  gpu_quad_init(quad);

  if (id < next_deleted_quad)
    next_deleted_quad = id; 

}

static int quad_vformat(void)
{
  if (!vformat) {
    vformat = GPU_vertex_format_create();
    GPU_vertex_format_add_attribute(vformat, "position_uv", 4, GL_HALF_FLOAT_OES);
  }

  return vformat;
}

static void gpu_quad_batch_init(GPUQuad *quad)
{
  if (!quad->batch) {
    quad->batch = GPU_batch_create();
    const ID_t ubuff = GPU_batch_uniform_buffer(quad->batch);
    GPU_uniformbuffer_add_4f(ubuff, "ProjMat", quad->ProjMatrix);    
    GPU_uniformbuffer_add_1f(ubuff, "alimit", quad->alimit);    
  }

  const ID_t vbuff = GPU_batch_vertex_buffer(quad->batch);
  GPU_batch_set_vertices_draw_count(quad->batch, 4);
  GPU_vertbuf_set_vertex_format(vbuff, quad_vformat());
  GPU_vertbuf_set_add_count(vbuff, 4);
  
#define VERTEX(x, y) (GPU_vertbuf_add_4(vbuff, ATTR_POSITION, x, y, ((x) + 1)*0.5f, (-(y) + 1)*0.5f))
#define QUAD(x, y, dx, dy) VERTEX(x, y); VERTEX(x+dx, y); VERTEX(x, y+dy); VERTEX(x+dx, y+dy);
  
  QUAD(-1, -1, 2, 2)
}

ID_t GPU_quad_create(void)
{
  const ID_t id = find_deleted_quad_id();
  GPUQuad *const quad = get_quad(id);
  quad->active = 1;
  gpu_quad_init(quad);
  gpu_quad_batch_init(quad);

  return id;
}

void GPU_quad_draw(const ID_t id)
{
  GPUQuad *const quad = get_quad(id);

  GPU_texture_bind(quad->texture, 0);
  GPU_shader_bind(quad->shader);
  GPU_uniformbuffer_activate(0);
  GPU_batch_draw(quad->batch, GL_TRIANGLE_STRIP, 1);  
}

void GPU_quad_set_texture(const ID_t id, const ID_t tex_id)
{
  get_quad(id)->texture = tex_id;
}

void GPU_quad_set_shader(const ID_t id, const ID_t shader_id)
{
  get_quad(id)->shader = shader_id;
}

void GPU_quad_set_alimit(const ID_t id, const float val)
{
  get_quad(id)->alimit = val;
}

