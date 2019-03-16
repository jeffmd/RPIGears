// gpu_batch.c

#include <stdio.h>

#include "gles3.h"

#include "gpu_index_buffer.h"
#include "gpu_vertex_format.h"
#include "gpu_vertex_buffer.h"

typedef struct {
  uint8_t active;
  GLuint vaoId;     // ID for vertex array object
  GLuint vbuffId;   // ID for vert buffer
  GLuint ibuffId;   // ID for index buffer
  GLuint progId;    // shader program ID
  GLuint vertices_draw_count; // number of vertices to draw
  GLuint indices_draw_count;  // number of indices to draw
} GPUBatch;

#define BATCH_MAX_COUNT 10

static GPUBatch batches[BATCH_MAX_COUNT];
static GPUBatch *next_deleted_batch = 0;

static GPUBatch *find_deleted_batch(void)
{
  GPUBatch *batch = next_deleted_batch;
  const GPUBatch *max_batch = batches + BATCH_MAX_COUNT;
  
  if((batch <= batches) | (batch >= max_batch))
    batch = batches + 1;

  for ( ; batch < max_batch; batch++) {
    if (batch->active == 0) {
      next_deleted_batch = batch + 1;
      break;
    }
  }

  if (batch >= max_batch) {
    printf("WARNING: No batches available\n");
    batch = batches;
  }
  printf("batch ID: %i ", batch - batches);
  return batch;
}

void GPU_batch_init(GPUBatch *batch)
{
  batch->active = 1;
  batch->vaoId = 0;
  batch->vbuffId = 0;
  batch->ibuffId = 0;
}

// create new batch
GPUBatch *GPU_batch_create(void)
{
  GPUBatch *batch = find_deleted_batch();
  GPU_batch_init(batch);
  printf("New batch ID:%p\n", batch);

  return batch;
}

void GPU_batch_delete(GPUBatch *batch, const int delete_buffers)
{
  if (batch->vaoId) {
    glDeleteVertexArrays(1, &batch->vaoId);
    batch->vaoId = 0;
  }
  
  if (delete_buffers) {
    if (batch->vbuffId) {
      GPU_vertbuf_delete(batch->vbuffId);
      batch->vbuffId = 0;
    }
    
    if (batch->ibuffId) {
      GPU_indexbuf_delete(batch->ibuffId);
      batch->ibuffId = 0;
    }
  }
  
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

void GPU_batch_set_index_buffer(GPUBatch *batch, const GLuint ibuffId)
{
  batch->ibuffId = ibuffId;  
}

void GPU_batch_set_vertex_buffer(GPUBatch *batch, const GLuint vbuffId) 
{
  batch->vbuffId = vbuffId;  
}

static void batch_bind(GPUBatch *batch)
{
  glGenVertexArrays(1, &batch->vaoId);
  glBindVertexArray(batch->vaoId);
  
  GPU_indexbuf_bind(batch->ibuffId);
  GPU_vertbuf_bind(batch->vbuffId);
}  

void GPU_batch_draw(GPUBatch *batch, const GLenum drawMode, const GLuint instances)
{
 
  if (!batch->vaoId)
    batch_bind(batch);
  else
    glBindVertexArray(batch->vaoId);
  
  if (drawMode == GL_POINTS)
    if (instances > 1 )
      glDrawArraysInstanced(drawMode, 0, batch->vertices_draw_count, instances);
    else
      glDrawArrays(drawMode, 0, batch->vertices_draw_count);
  else
    if (instances > 1)
      glDrawElementsInstanced(drawMode, batch->indices_draw_count, GL_UNSIGNED_SHORT, GPU_indexbuf_get_index(batch->ibuffId), instances);
    else
      glDrawElements(drawMode, batch->indices_draw_count, GL_UNSIGNED_SHORT, GPU_indexbuf_get_index(batch->ibuffId));
}

