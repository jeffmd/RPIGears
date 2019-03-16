// gpu_index_buffer.c

#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"

typedef struct {
  uint8_t active;           // not zero if vertex buffer is not deleted
  GLuint max_count;         // max number of verts in data buffer
  GLuint idx;               // index into data during update
  GLshort *data;            // pointer to data buffer in cpu ram
  GLvoid *index;            // pointer(vertex array) or 0(using vbo) to index data
  GLuint ibo_id;            // 0 indicates using client ram or not allocated yet
  GLenum usage;             // usage hint for GL optimisation
  uint8_t ready;            // not zero if ready for adding data to buffer
} IndexBuffer;

#define INDEX_BUFFER_MAX_COUNT 10

static IndexBuffer index_buffers[INDEX_BUFFER_MAX_COUNT];
static IndexBuffer *next_deleted_index_buffer = 0;

static IndexBuffer *find_deleted_index_buffer(void)
{
  IndexBuffer *ibuff = next_deleted_index_buffer;
  const IndexBuffer *max_ibuff = index_buffers + INDEX_BUFFER_MAX_COUNT;
  
  if((ibuff <= index_buffers) | (ibuff >= max_ibuff))
    ibuff = index_buffers + 1;

  for ( ; ibuff < max_ibuff; ibuff++) {
    if (ibuff->active == 0) {
      next_deleted_index_buffer = ibuff + 1;
      break;
    }
  }

  if (ibuff == max_ibuff) {
    printf("WARNING: No index buffers available\n");
    ibuff = index_buffers;
  }

  return ibuff;
}

void GPU_indexbuf_init(IndexBuffer *ibuff)
{
  ibuff->active = 1;
  ibuff->max_count = 0;
  ibuff->data = 0;
  ibuff->index = 0;
  ibuff->ibo_id = 0;
  ibuff->usage = GL_STATIC_DRAW;
  ibuff->ready = 0;
  
}

// create new indexbuffer
IndexBuffer *GPU_indexbuf_create(void)
{
  IndexBuffer *ibuff = find_deleted_index_buffer();
  GPU_indexbuf_init(ibuff);
  printf("New indexbuf ID:%p\n", ibuff);

  return ibuff;
}

void GPU_indexbuf_delete(IndexBuffer *ibuff)
{
  if (ibuff->data) {
    free(ibuff->data);
    ibuff->data = 0;
  }
  
  if (ibuff->ibo_id) {
    glDeleteBuffers(1, &ibuff->ibo_id);
    ibuff->ibo_id = 0;
  }
  
  ibuff->active = 0;
  
  if (ibuff < next_deleted_index_buffer)
    next_deleted_index_buffer = ibuff; 
}

// begin data update ( index max count )
void GPU_indexbuf_begin_update(IndexBuffer *ibuff, const GLuint max_count)
{
  // allocate heap storage for data
  if (!ibuff->data)
    ibuff->data = calloc(max_count, sizeof(GLshort));
  else if (max_count > ibuff->max_count) {
    ibuff->data = realloc(ibuff->data, max_count * sizeof(GLshort));
  }

  ibuff->max_count = max_count;
  ibuff->idx = 0;
  // ready to receive data updates in buffer storage
  ibuff->ready = 1;
}

void GPU_indexbuf_add_3(IndexBuffer *ibuff, const GLshort val1, const GLshort val2, const GLshort val3)
{
  if (ibuff->ready) {
    ibuff->data[ibuff->idx++] = val1;
    ibuff->data[ibuff->idx++] = val2;
    ibuff->data[ibuff->idx++] = val3;
  }
}

void GPU_indexbuf_use_VBO(IndexBuffer *ibuff)
{
  if (!ibuff->ibo_id) {
    glGenBuffers(1, &ibuff->ibo_id);
  }
}

// set VAO
void GPU_indexbuf_bind(IndexBuffer *ibuff)
{
  if (ibuff->ready) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuff->ibo_id);
    ibuff->index = ibuff->data;
    if (ibuff->ibo_id) {
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * ibuff->max_count, ibuff->data, ibuff->usage);
      ibuff->index = 0;
    }
  }
}

GLvoid *GPU_indexbuf_get_index(IndexBuffer *ibuff)
{
  return ibuff->index;
}
