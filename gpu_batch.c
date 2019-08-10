// gpu_batch.c

#include <stdio.h>

#include "gles3.h"

#include "static_array.h"
#include "gpu_index_buffer.h"
#include "gpu_vertex_format.h"
#include "gpu_vertex_buffer.h"
#include "gpu_shader.h"
#include "gpu_uniform_buffer.h"

typedef struct {
  GPUShader *shader;        // the shader used for binding
  int modid;
  GLuint vaoId;               // ID for vertex array object
} ShaderVAO;

#define SHADER_CACHE_MAX_COUNT 5

typedef struct {
  uint8_t active;
  ShaderVAO shaders[SHADER_CACHE_MAX_COUNT];        // shader VAO cache
  uint8_t next_index;         // next index to use for a new shader binding
  uint8_t active_index;       // the index to the shader used for current binding
  int vbuff;                  // ID for vert buffer
  int ibuff;                  // ID for index buffer
  int ubuff;                  // ID for uniform buffer
  GLuint vertices_draw_count; // number of vertices to draw
  GLuint indices_draw_count;  // number of indices to draw
} GPUBatch;

#define BATCH_MAX_COUNT 10

static GPUBatch batches[BATCH_MAX_COUNT];
static int next_deleted_batch;

static inline int find_deleted_batch_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_batch, batches, BATCH_MAX_COUNT, GPUBatch, "batch");
}

static GPUBatch *get_batch(int id)
{
  if ((id < 0) | (id >= BATCH_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad BATCH id, using default id: 0\n");
  }
    
  return batches + id;
}


static int batch_next_index(GPUBatch *batch)
{
  if (batch->next_index >= SHADER_CACHE_MAX_COUNT)
    batch->next_index = 0;
      
  return batch->next_index++;
}

static void batch_clear_shaders(GPUBatch *batch)
{
  for (int index = 0; index < SHADER_CACHE_MAX_COUNT; index++) {
    // don't need to clear shader, just modid to force rebinding
    batch->shaders[index].modid = 0;
  }  
}

static int batch_needs_binding(GPUBatch *batch, GPUShader *shader)
{
  int index = 0;
  int needs_binding = 1;
  int modid = GPU_shader_modid(shader);
  
  for ( ; index < SHADER_CACHE_MAX_COUNT; index++) {
    if (batch->shaders[index].shader == shader) {
      if (batch->shaders[index].modid == modid)
        needs_binding = 0; // no changes in shader
      break;
    }
    
    if (batch->shaders[index].shader == 0) {
      break;
    }
  }
  
  if (index >= SHADER_CACHE_MAX_COUNT) {
    index = batch_next_index(batch);
  }
  
  batch->shaders[index].shader = shader;
  batch->shaders[index].modid = modid;
  batch->active_index = index;
  
  return needs_binding;
}

static void batch_unbind(GPUBatch *batch)
{
  batch->shaders[batch->active_index].shader = 0;
}

static void batch_init(GPUBatch *batch)
{
  batch->vbuff = 0;
  batch->ibuff = 0;
  batch->ubuff = 0;

  batch->active_index = 0;
  batch->next_index = 1;
  
  batch_unbind(batch);
}

// create new batch
int GPU_batch_create(void)
{
  const int id = find_deleted_batch_id();
  GPUBatch *batch = get_batch(id);
  batch->active = 1;
  batch_init(batch);

  return id;
}

void GPU_batch_delete(int id, const int delete_buffers)
{

  GPUBatch *batch = get_batch(id);

  if (delete_buffers) {
    if (batch->vbuff) {
      GPU_vertbuf_delete(batch->vbuff);
    }

    if (batch->ibuff) {
      GPU_indexbuf_delete(batch->ibuff);
    }

    if (batch->ubuff) {
      GPU_uniformbuffer_delete(batch->ubuff);
    }
  }
  
  batch_init(batch);
  batch->active = 0;

  if (id < next_deleted_batch)
    next_deleted_batch = id;
}

void GPU_batch_set_indices_draw_count(int id, const int count)
{
  get_batch(id)->indices_draw_count = count;
}

void GPU_batch_set_vertices_draw_count(int id, const int count)
{
  get_batch(id)->vertices_draw_count = count;
}

void GPU_batch_set_index_buffer(int id, int ibuff)
{
  get_batch(id)->ibuff = ibuff;
}

void GPU_batch_set_vertex_buffer(int id, int vbuff)
{
  get_batch(id)->vbuff = vbuff;
}

void GPU_batch_set_uniform_buffer(int id, int ubuff)
{
  get_batch(id)->ubuff = ubuff;
}

int GPU_batch_vertex_buffer(int id)
{
  GPUBatch *batch = get_batch(id);

  if (!batch->vbuff)
    batch->vbuff = GPU_vertbuf_create();

  return batch->vbuff;
}

int GPU_batch_index_buffer(int id)
{
  GPUBatch *batch = get_batch(id);

  if (!batch->ibuff)
    batch->ibuff = GPU_indexbuf_create();

  return batch->ibuff;
}

int GPU_batch_uniform_buffer(int id)
{
  GPUBatch *batch = get_batch(id);

  if (!batch->ubuff)
    batch->ubuff = GPU_uniformbuffer_create();

  return batch->ubuff;
}

static void batch_update_bindings(GPUBatch *batch)
{
  ShaderVAO *shadervao = &batch->shaders[batch->active_index];
  
  if (!shadervao->vaoId)
    glGenVertexArrays(1, &shadervao->vaoId);
    
  glBindVertexArray(shadervao->vaoId);

  if (batch->ibuff)
    GPU_indexbuf_bind(batch->ibuff);

  if (batch->vbuff)
    GPU_vertbuf_bind(batch->vbuff);
  
  printf("batch update bindings\n");
}

static void batch_bind(GPUBatch *batch)
{
  GPUShader *shader = GPU_shader_active_shader();
  
  if ((batch->shaders[batch->active_index].shader != shader)
    || (batch->shaders[batch->active_index].modid != GPU_shader_modid(shader))) {
    if (batch_needs_binding(batch, shader)) {
      batch_update_bindings(batch);
    }
  }

  glBindVertexArray(batch->shaders[batch->active_index].vaoId);
}

void GPU_batch_use_BO(int id)
{
  GPUBatch *batch = get_batch(id);

  if (batch->ibuff)
    GPU_indexbuf_use_BO(batch->ibuff);

  if (batch->vbuff)
    GPU_vertbuf_use_BO(batch->vbuff);

  batch_clear_shaders(batch);
}

void GPU_batch_no_BO(int id)
{
  GPUBatch *batch = get_batch(id);

  if (batch->ibuff)
    GPU_indexbuf_no_BO(batch->ibuff);

  if (batch->vbuff)
    GPU_vertbuf_no_BO(batch->vbuff);

  batch_clear_shaders(batch);
}

void GPU_batch_draw(int id, const GLenum drawMode, const GLuint instances)
{
  GPUBatch *batch = get_batch(id);

  batch_bind(batch);

  if (batch->ubuff)
    GPU_uniformbuffer_update(batch->ubuff);
  
  GPU_uniformbuffer_update_active();
  
  if ((drawMode == GL_POINTS) || !batch->ibuff) {
    if (instances > 1 )
      glDrawArraysInstanced(drawMode, 0, batch->vertices_draw_count, instances);
    else
      glDrawArrays(drawMode, 0, batch->vertices_draw_count);
  }
  else {
    if (instances > 1)
      glDrawElementsInstanced(drawMode, batch->indices_draw_count, GL_UNSIGNED_SHORT, GPU_indexbuf_get_index(batch->ibuff), instances);
    else
      glDrawElements(drawMode, batch->indices_draw_count, GL_UNSIGNED_SHORT, GPU_indexbuf_get_index(batch->ibuff));
  }
}

