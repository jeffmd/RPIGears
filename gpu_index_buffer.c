// gpu_index_buffer.c

#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"

#include "static_array.h"

typedef struct {
  uint8_t active;          // not zero if vertex buffer is not deleted
  GLuint alloc_count;       // number of verts allocated in data buffer
  GLuint add_count;         // number of verts to add to data buffer when resize occurs
  GLuint idx;               // index into data during update
  GLshort *data;            // pointer to data buffer in cpu ram
  GLvoid *index;            // pointer(vertex array) or 0(using vbo) to index data
  GLuint ibo_id;            // 0 indicates using client ram or not allocated yet
  GLenum usage;             // usage hint for GL optimisation
  uint8_t ready;           // not zero if ready for adding data to buffer
} GPUIndexBuffer;

#define INDEX_BUFFER_MAX_COUNT 10
#define DEFAULT_COUNT 100

static GPUIndexBuffer index_buffers[INDEX_BUFFER_MAX_COUNT];
static int next_deleted_index_buffer;

static inline int find_deleted_index_buffer_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_index_buffer, index_buffers,
                            INDEX_BUFFER_MAX_COUNT, GPUIndexBuffer, "Index Buffer");
}

static GPUIndexBuffer *get_index_buffer(int id)
{
  if ((id < 0) | (id >= INDEX_BUFFER_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Index buffer id, using default id: 0\n");
  }
    
  return index_buffers + id;
}

static void delete_data(GPUIndexBuffer *ibuff)
{
  if (ibuff->data) {
    free(ibuff->data);
    ibuff->data = 0;
  }
}

static void delete_ibo(GPUIndexBuffer *ibuff)
{
  if (ibuff->ibo_id) {
    glDeleteBuffers(1, &ibuff->ibo_id);
    ibuff->ibo_id = 0;
  }
}

static void indexbuf_init(GPUIndexBuffer *ibuff)
{
  ibuff->usage = GL_STATIC_DRAW;
  ibuff->ready = 0;
  
  delete_ibo(ibuff);
  delete_data(ibuff);
  
}


// create new GPUIndexBuffer
int GPU_indexbuf_create(void)
{
  const int id = find_deleted_index_buffer_id();
  GPUIndexBuffer *ibuff = get_index_buffer(id);
  ibuff->active = 1;
  indexbuf_init(ibuff);

  return id;
}

void GPU_indexbuf_delete(const int id)
{
  GPUIndexBuffer *ibuff = get_index_buffer(id);

  ibuff->active = 0;
  indexbuf_init(ibuff);
    
  if (id < next_deleted_index_buffer)
    next_deleted_index_buffer = id; 
}

void GPU_indexbuf_set_add_count(const int id, const GLuint count)
{
  get_index_buffer(id)->add_count = count;
}

void GPU_indexbuf_set_start(const int id, const GLuint start)
{
  get_index_buffer(id)->idx = start;
}


static void indexbuf_make_ready(GPUIndexBuffer *ibuff)
{
  if (!ibuff->add_count) {
    ibuff->add_count = DEFAULT_COUNT;
  }
  
  if (!ibuff->alloc_count) {
    ibuff->alloc_count = ibuff->add_count;
  }
  
  // allocate heap storage for data
  if (!ibuff->data) {
    ibuff->data = calloc(ibuff->alloc_count, sizeof(GLshort));
  }

  // ready to receive data updates in buffer storage
  ibuff->ready = 1;
}

static GLuint indexbuf_idx(GPUIndexBuffer *ibuff)
{
  GLuint idx = ibuff->idx;

  if (idx >= ibuff->alloc_count) {
    const int new_count = ibuff->alloc_count + ibuff->add_count;

    void *data = realloc(ibuff->data, new_count * sizeof(GLshort));
    
    if (data) {
      printf("increased index buffer data memory allocation, old alloc:%i new alloc:%i\n", ibuff->alloc_count, new_count);
      ibuff->data = data;
      ibuff->alloc_count = new_count;
    }
    else {
      // allocation failed so recycle last index to get gracefull fail
      idx = ibuff->alloc_count - 1;
      printf("ERROR: increasing index buffer data memory allocation failed\n");
    }
  }
  
  return idx;
}

void GPU_indexbuf_add(const int id, const GLshort val)
{
  GPUIndexBuffer *ibuff = get_index_buffer(id);

  if (!ibuff->ready) {
    indexbuf_make_ready(ibuff);
  }
 
  if (ibuff->ready) {
    const GLuint idx = indexbuf_idx(ibuff);
    
    ibuff->data[idx] = val;
    ibuff->idx = idx + 1;
  }
}

void GPU_indexbuf_use_BO(const int id)
{
  GPUIndexBuffer *ibuff = get_index_buffer(id);

  if (!ibuff->ibo_id) {
    glGenBuffers(1, &ibuff->ibo_id);
  }
}

void GPU_indexbuf_no_BO(const int id)
{
  delete_ibo(get_index_buffer(id));
}

// set VAO
void GPU_indexbuf_bind(const int id)
{
  GPUIndexBuffer *ibuff = get_index_buffer(id);

  if (ibuff->ready) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuff->ibo_id);
    ibuff->index = ibuff->data;
    if (ibuff->ibo_id) {
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * ibuff->alloc_count, ibuff->data, ibuff->usage);
      ibuff->index = 0;
    }
  }
}

GLvoid *GPU_indexbuf_get_index(const int id)
{
  return get_index_buffer(id)->index;
}
