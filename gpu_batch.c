// gpu_batch.c

#include <stdio.h>

#include "gles3.h"

#include "gpu_index_buffer.h"
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
static uint16_t next_deleted_batch = 0;

static GLuint find_deleted_batch(void)
{
  GLuint id = next_deleted_batch;
  
  if((id == 0) | (id >= BATCH_MAX_COUNT))
    id = 1;

  for ( ; id < BATCH_MAX_COUNT; id++) {
    if (batches[id].active == 0) {
      next_deleted_batch = id + 1;
      break;
    }
  }

  if (id == BATCH_MAX_COUNT) {
    printf("WARNING: No batches available\n");
    id = 0;
  }

  return id;
}

void GPU_batch_init(const GLuint batch_id)
{
  GPUBatch *batch = &batches[batch_id];
  
  batch->active = 1;
  batch->vaoId = 0;
  batch->vbuffId = 0;
  batch->ibuffId = 0;
  
}

// create new batch
GLuint GPU_batch_create(void)
{
  const GLuint batch_id = find_deleted_batch();
  GPU_batch_init(batch_id);
  printf("New batch ID:%i\n", batch_id);

  return batch_id;
}

void GPU_batch_delete(const GLuint batch_id, const int delete_buffers)
{
  GPUBatch *batch = &batches[batch_id];

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
  
  if (batch_id < next_deleted_batch)
    next_deleted_batch = batch_id; 
}

void GPU_batch_set_indices_draw_count(const GLuint batch_id, const int count)
{
  GPUBatch *batch = &batches[batch_id];
  
  batch->indices_draw_count = count;
  
}

void GPU_batch_set_vertices_draw_count(const GLuint batch_id, const int count)
{
  GPUBatch *batch = &batches[batch_id];
  
  batch->vertices_draw_count = count;
}

void GPU_batch_set_index_buffer(const GLuint batch_id, const GLuint ibuffId)
{
  GPUBatch *batch = &batches[batch_id];

  batch->ibuffId = ibuffId;  
}

void GPU_batch_set_vertex_buffer(const GLuint batch_id, const GLuint vbuffId) 
{
  GPUBatch *batch = &batches[batch_id];
  
  batch->vbuffId = vbuffId;  
}

static void batch_set_VAO(GPUBatch *batch)
{
  glGenVertexArrays(1, &batch->vaoId);
  glBindVertexArray(batch->vaoId);
  
  GPU_indexbuf_set_VAO(batch->ibuffId);
  GPU_vertbuf_set_VAO(batch->vbuffId);
}  

void GPU_batch_draw(const GLuint batch_id, const GLenum drawMode, const GLuint instances)
{
  GPUBatch *batch = &batches[batch_id];
 
  if (!batch->vaoId)
    batch_set_VAO(batch);
    
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

