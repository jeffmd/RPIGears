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
  short shader;               // the shader used for binding
  uint8_t modid;
  GLuint vaoId;               // ID for vertex array object
} ShaderVAO;

#define SHADER_CACHE_MAX_COUNT 5

typedef struct {
  uint8_t active;
  ShaderVAO shaders[SHADER_CACHE_MAX_COUNT]; // shader VAO cache
  uint8_t next_shader;        // next index to use for a new shader binding
  uint8_t active_shader;      // the index to the shader used for current binding
  short vbuff;                // ID for vert buffer
  short ibuff;                // ID for index buffer
  short ubuff;                // ID for uniform buffer
  uint16_t start;             // start index in buffer for draw
  GLuint vertices_draw_count; // number of vertices to draw
  GLuint indices_draw_count;  // number of indices to draw
} GPUBatch;

typedef struct {
  uint8_t active;
  short batch;
  uint16_t start;
  uint16_t count;
} GPUBatchPart;

#define BATCH_MAX_COUNT 10
#define BATCHPART_MAX_COUNT 100

static GPUBatch batches[BATCH_MAX_COUNT];
static short next_deleted_batch;

static GPUBatchPart batch_parts[BATCHPART_MAX_COUNT];
static short next_deleted_batchpart;

static inline short find_deleted_batch_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_batch, batches, BATCH_MAX_COUNT, GPUBatch, "batch");
}

static GPUBatch *get_batch(short id)
{
  if ((id < 0) | (id >= BATCH_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad BATCH id, using default id: 0\n");
  }

  return batches + id;
}

static int batch_next_shader(GPUBatch *batch)
{
  if (batch->next_shader >= SHADER_CACHE_MAX_COUNT)
    batch->next_shader = 0;
      
  return batch->next_shader++;
}

static void batch_clear_shaders(GPUBatch *batch)
{
  for (int index = 0; index < SHADER_CACHE_MAX_COUNT; index++) {
    // don't need to clear shader, just modid to force rebinding
    batch->shaders[index].modid = 0;
  }  
}

static int batch_needs_binding(GPUBatch *batch, const short shader)
{
  int index = 0;
  int needs_binding = 1;
  uint8_t modid = GPU_shader_modid(shader);
  
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
    index = batch_next_shader(batch);
  }
  
  batch->shaders[index].shader = shader;
  batch->shaders[index].modid = modid;
  batch->active_shader = index;
  
  return needs_binding;
}

static void batch_unbind(GPUBatch *batch)
{
  batch->shaders[batch->active_shader].shader = 0;
}

static void batch_init(GPUBatch *batch)
{
  batch->vbuff = 0;
  batch->ibuff = 0;
  batch->ubuff = 0;

  batch->active_shader = 0;
  batch->next_shader = 1;
  
  batch_unbind(batch);
}

// create new batch
short GPU_batch_create(void)
{
  const short id = find_deleted_batch_id();
  GPUBatch *const batch = get_batch(id);
  batch->active = 1;
  batch_init(batch);

  return id;
}

void GPU_batch_delete(const short id, const int delete_buffers)
{

  GPUBatch *const batch = get_batch(id);

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

void GPU_batch_set_indices_draw_count(const short id, const int count)
{
  get_batch(id)->indices_draw_count = count;
}

void GPU_batch_set_vertices_draw_count(const short id, const int count)
{
  get_batch(id)->vertices_draw_count = count;
}

void GPU_batch_set_index_buffer(const short id, const short ibuff)
{
  get_batch(id)->ibuff = ibuff;
}

void GPU_batch_set_vertex_buffer(const short id, const short vbuff)
{
  get_batch(id)->vbuff = vbuff;
}

void GPU_batch_set_uniform_buffer(const short id, const short ubuff)
{
  get_batch(id)->ubuff = ubuff;
}

short GPU_batch_vertex_buffer(const short id)
{
  GPUBatch *const batch = get_batch(id);

  if (!batch->vbuff)
    batch->vbuff = GPU_vertbuf_create();

  return batch->vbuff;
}

short GPU_batch_index_buffer(const short id)
{
  GPUBatch *const batch = get_batch(id);

  if (!batch->ibuff)
    batch->ibuff = GPU_indexbuf_create();

  return batch->ibuff;
}

short GPU_batch_uniform_buffer(const short id)
{
  GPUBatch *const batch = get_batch(id);

  if (!batch->ubuff)
    batch->ubuff = GPU_uniformbuffer_create();

  return batch->ubuff;
}

static void batch_update_bindings(GPUBatch *batch)
{
  ShaderVAO *const shadervao = &batch->shaders[batch->active_shader];
  
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
  const short shader = GPU_shader_active_shader();
  
  if ((batch->shaders[batch->active_shader].shader != shader)
    || (batch->shaders[batch->active_shader].modid != GPU_shader_modid(shader))) {
    if (batch_needs_binding(batch, shader)) {
      batch_update_bindings(batch);
    }
  }

  glBindVertexArray(batch->shaders[batch->active_shader].vaoId);
}

void GPU_batch_use_BO(const short id)
{
  GPUBatch *const batch = get_batch(id);

  if (batch->ibuff)
    GPU_indexbuf_use_BO(batch->ibuff);

  if (batch->vbuff)
    GPU_vertbuf_use_BO(batch->vbuff);

  batch_clear_shaders(batch);
}

void GPU_batch_no_BO(const short id)
{
  GPUBatch *const batch = get_batch(id);

  if (batch->ibuff)
    GPU_indexbuf_no_BO(batch->ibuff);

  if (batch->vbuff)
    GPU_vertbuf_no_BO(batch->vbuff);

  batch_clear_shaders(batch);
}

void GPU_batch_draw(const short id, const GLenum drawMode, const GLuint instances)
{
  GPUBatch *const batch = get_batch(id);

  batch_bind(batch);

  if (batch->ubuff)
    GPU_uniformbuffer_update(batch->ubuff);
  
  GPU_uniformbuffer_update_active();
  
  if ((drawMode == GL_POINTS) || !batch->ibuff) {
    if (instances > 1 )
      glDrawArraysInstanced(drawMode, batch->start, batch->vertices_draw_count, instances);
    else
      glDrawArrays(drawMode, batch->start, batch->vertices_draw_count);
  }
  else {
    if (instances > 1)
      glDrawElementsInstanced(drawMode, batch->indices_draw_count, GL_UNSIGNED_SHORT, GPU_indexbuf_start(batch->ibuff), instances);
    else
      glDrawElements(drawMode, batch->indices_draw_count, GL_UNSIGNED_SHORT, GPU_indexbuf_start(batch->ibuff));
  }
}

void GPU_batch_set_draw_start(const short id, uint16_t start)
{
  get_batch(id)->start = start;
}

static inline short find_deleted_batch_part(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_batchpart, batch_parts, BATCHPART_MAX_COUNT, GPUBatchPart, "batch part");
}

static GPUBatchPart *get_batch_part(short id)
{
  if ((id < 0) | (id >= BATCHPART_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad BATCH PART id, using default id: 0\n");
  }
    
  return batch_parts + id;
}

static void batch_part_init(GPUBatchPart *batch_part)
{
  GPUBatch *const batch = get_batch(batch_part->batch);
  
  if (!batch->ibuff) {
    batch_part->start = GPU_vertbuf_index(batch->vbuff);
  }
  else {
    batch_part->start = GPU_indexbuf_index(batch->ibuff);
  }

}

short GPU_batch_part_create(const short batch)
{
  const short id = find_deleted_batch_part();
  GPUBatchPart *const batch_part = get_batch_part(id);
  batch_part->active = 1;
  batch_part->batch = batch;
  batch_part_init(batch_part);

  return id;
}

void GPU_batch_part_end(const short part)
{
  GPUBatchPart *const batch_part = get_batch_part(part);
  GPUBatch *const batch = get_batch(batch_part->batch);

  if (!batch->ibuff) {
    batch_part->count = GPU_vertbuf_index(batch->vbuff) - batch_part->start;
  }
  else {
    batch_part->count = GPU_indexbuf_index(batch->ibuff) - batch_part->start;
  }
}

void GPU_batch_part_draw(const short part, const GLenum drawMode, const GLuint instances)
{
  GPUBatchPart *const batch_part = get_batch_part(part);
  GPUBatch *const batch = get_batch(batch_part->batch);

  batch->start = batch_part->start;

  if (!batch->ibuff) {
    batch->vertices_draw_count = batch_part->count;
  }
  else {
    batch->indices_draw_count = batch_part->count;
  }

  GPU_batch_draw(batch_part->batch, drawMode, instances);
}

