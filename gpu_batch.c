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
  uint8_t active;
  GLuint vaoId;               // ID for vertex array object
  GPUVertBuffer *vbuff;       // ID for vert buffer
  GPUIndexBuffer *ibuff;      // ID for index buffer
  GPUUniformBuffer *ubuff;    // ID for uniform buffer
  GPUShader *shader;          // shader program ID
  GLuint vertices_draw_count; // number of vertices to draw
  GLuint indices_draw_count;  // number of indices to draw
} GPUBatch;

#define BATCH_MAX_COUNT 10

static GPUBatch batches[BATCH_MAX_COUNT];
static GPUBatch *next_deleted_batch = 0;

static inline GPUBatch *find_deleted_batch(void)
{
  return ARRAY_FIND_DELETED(next_deleted_batch, batches, BATCH_MAX_COUNT, "batch");
}

static void batch_init(GPUBatch *batch)
{
  batch->vaoId = 0;
  batch->vbuff = 0;
  batch->ibuff = 0;
  batch->ubuff = 0;
  batch->shader = 0;
}

// create new batch
GPUBatch *GPU_batch_create(void)
{
  GPUBatch *batch = find_deleted_batch();
  batch->active = 1;
  batch_init(batch);

  return batch;
}

void GPU_batch_delete(GPUBatch *batch, const int delete_buffers)
{
  if (batch->vaoId) {
    glDeleteVertexArrays(1, &batch->vaoId);
  }

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

  if (batch < next_deleted_batch)
    next_deleted_batch = batch;
}

void GPU_batch_set_indices_draw_count(GPUBatch *batch, const int count)
{
  batch->indices_draw_count = count;
}

void GPU_batch_set_vertices_draw_count(GPUBatch *batch, const int count)
{
  batch->vertices_draw_count = count;
}

void GPU_batch_set_index_buffer(GPUBatch *batch, GPUIndexBuffer *ibuff)
{
  batch->ibuff = ibuff;
}

void GPU_batch_set_vertex_buffer(GPUBatch *batch, GPUVertBuffer *vbuff)
{
  batch->vbuff = vbuff;
}

void GPU_batch_set_uniform_buffer(GPUBatch *batch, GPUUniformBuffer *ubuff)
{
  batch->ubuff = ubuff;
}

GPUVertBuffer *GPU_batch_vertex_buffer(GPUBatch *batch)
{
  if (!batch->vbuff)
    batch->vbuff = GPU_vertbuf_create();

  return batch->vbuff;
}

GPUIndexBuffer *GPU_batch_index_buffer(GPUBatch *batch)
{
  if (!batch->ibuff)
    batch->ibuff = GPU_indexbuf_create();

  return batch->ibuff;
}

GPUUniformBuffer *GPU_batch_uniform_buffer(GPUBatch *batch)
{
  if (!batch->ubuff)
    batch->ubuff = GPU_uniformbuffer_create();

  return batch->ubuff;
}

static void batch_bind(GPUBatch *batch)
{
  glGenVertexArrays(1, &batch->vaoId);
  glBindVertexArray(batch->vaoId);

  GPU_indexbuf_bind(batch->ibuff);
  GPU_vertbuf_bind(batch->vbuff);
}

void GPU_batch_draw(GPUBatch *batch, const GLenum drawMode, const GLuint instances)
{

  if (!batch->vaoId)
    batch_bind(batch);
  else
    glBindVertexArray(batch->vaoId);

  if (batch->ubuff)
    GPU_uniformbuffer_update(batch->ubuff);

  if (drawMode == GL_POINTS)
    if (instances > 1 )
      glDrawArraysInstanced(drawMode, 0, batch->vertices_draw_count, instances);
    else
      glDrawArrays(drawMode, 0, batch->vertices_draw_count);
  else
    if (instances > 1)
      glDrawElementsInstanced(drawMode, batch->indices_draw_count, GL_UNSIGNED_SHORT, GPU_indexbuf_get_index(batch->ibuff), instances);
    else
      glDrawElements(drawMode, batch->indices_draw_count, GL_UNSIGNED_SHORT, GPU_indexbuf_get_index(batch->ibuff));
}

