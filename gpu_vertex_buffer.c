// gpu_vertex_buffer.c

#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"
#include "gpu_vertex_format.h"
#include "static_array.h"

typedef struct {
  uint8_t active;            // not zero if vertex buffer is not deleted
  GLuint data_idx[VERT_ATTRIB_MAX];     // current index in data buffer for writing
  GLuint alloc_count;        // number of verts allocated in data buffer
  GLuint add_count;          // number of verts to add to data buffer when resize occurs
  void *data;               // pointer to data buffer in cpu ram
  GPUVertFormat *vformat;  // vertex format ID
  GLuint vbo_id;            // 0 indicates using client ram or not allocated yet
  GLenum usage;             // usage hint for GL optimisation
  uint8_t ready;           // not zero if ready for adding data to buffer
} GPUVertBuffer;

#define VERT_BUFFER_MAX_COUNT 10
#define DEFAULT_COUNT 100

static GPUVertBuffer vert_buffers[VERT_BUFFER_MAX_COUNT];
static GPUVertBuffer *next_deleted_vert_buffer = 0;

static inline GPUVertBuffer *find_deleted_vert_buffer(void)
{
  return ARRAY_FIND_DELETED(next_deleted_vert_buffer, vert_buffers,
                            VERT_BUFFER_MAX_COUNT, "vertex buffer");
}

static void delete_data(GPUVertBuffer *vbuff)
{
  if (vbuff->data) {
    free(vbuff->data);
    vbuff->data = 0;
  }
}

static void delete_vbo(GPUVertBuffer *vbuff)
{
  if (vbuff->vbo_id) {
    glDeleteBuffers(1, &vbuff->vbo_id);
    vbuff->vbo_id = 0;
  }
}


static void vertbuf_init(GPUVertBuffer *vbuff)
{
  vbuff->vformat = 0;
  vbuff->usage = GL_STATIC_DRAW;
  vbuff->ready = 0;

  delete_vbo(vbuff);
  delete_data(vbuff);
}

// create new GPUVertBuffer
GPUVertBuffer *GPU_vertbuf_create(void)
{
  GPUVertBuffer *vbuff = find_deleted_vert_buffer();
  vbuff->active = 1;
  vertbuf_init(vbuff);

  return vbuff;
}

void GPU_vertbuf_delete(GPUVertBuffer *vbuff)
{
  vbuff->active = 0;
  vertbuf_init(vbuff);

  if (vbuff < next_deleted_vert_buffer)
    next_deleted_vert_buffer = vbuff;
}

void GPU_vertbuf_set_vertex_format(GPUVertBuffer *vbuff, GPUVertFormat *vformat)
{
  vbuff->vformat = vformat;
}

void GPU_vertbuf_set_add_count(GPUVertBuffer *vbuff, const GLuint count)
{
  vbuff->add_count = count;
}

void GPU_vertbuf_set_start(GPUVertBuffer *vbuff, const GLuint start)
{
  for (GLuint Idx = 0; Idx < VERT_ATTRIB_MAX; Idx++) {
    vbuff->data_idx[Idx] = start;
  }
}

// make vertex buffer ready for accepting data ( vertex max count ) - no more attributes can be added
static void vertbuf_make_ready(GPUVertBuffer *vbuff)
{
  if (vbuff->vformat) {
    const uint8_t stride = GPU_vertex_format_stride(vbuff->vformat);

    if (stride > 0) {
      if (!vbuff->add_count) {
        vbuff->add_count = DEFAULT_COUNT;
      }
      
      if (!vbuff->alloc_count) {
        vbuff->alloc_count = vbuff->add_count;
      }
            
      // allocate heap storage for data
      if (!vbuff->data) {
        vbuff->data = calloc(vbuff->alloc_count, stride);
      }

      // ready to receive data updates in buffer storage
      vbuff->ready = 1;
    }
    else {
      printf("ERROR: vertex format has no attributes\n");
    }
  }
  else {
    printf("ERROR: vertex buffer not ready for updating, there is no vertex format attached\n");    
  }
}

static void *vertbuf_attr_data(GPUVertBuffer *vbuff, const GLuint attribute_id)
{
  GLuint idx = vbuff->data_idx[attribute_id];
  const uint8_t stride = GPU_vertex_format_stride(vbuff->vformat);
  
  if (idx >= vbuff->alloc_count) {
    const int new_count = vbuff->alloc_count + vbuff->add_count;

    void *data = realloc(vbuff->data, new_count * stride);
    
    if (data) {
      printf("increased vertex buffer data memory allocation, old alloc:%i new alloc:%i\n", vbuff->alloc_count, new_count);
      vbuff->data = data;
      vbuff->alloc_count = new_count;
    }
    else {
      // allocation failed so recycle last index to get gracefull fail
      idx = vbuff->alloc_count - 1;
      vbuff->data_idx[attribute_id] = idx;
      printf("ERROR: increasing vertex buffer data memory allocation failed\n");
    }
    
  }
  
  return vbuff->data + (idx * stride) + GPU_vertex_format_offset(vbuff->vformat, attribute_id);  
}

void GPU_vertbuf_add_4(GPUVertBuffer *vbuff, const GLuint attribute_id, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4)
{
  if (!vbuff->ready) {
    vertbuf_make_ready(vbuff);
  }
  
  if (vbuff->ready) {
    void *data = vertbuf_attr_data(vbuff, attribute_id);
    GPU_vertex_format_add_4(vbuff->vformat, attribute_id, data, val1, val2, val3, val4);
    vbuff->data_idx[attribute_id]++;
  }
  else {
    printf("ERROR: vertex buffer not ready for adding data\n");    
  }
}

void GPU_vertbuf_use_BO(GPUVertBuffer *vbuff)
{
  if (!vbuff->vbo_id) {
    glGenBuffers(1, &vbuff->vbo_id);
  }
}

void GPU_vertbuf_no_BO(GPUVertBuffer *vbuff)
{
  delete_vbo(vbuff);
}

void GPU_vertbuf_bind(GPUVertBuffer *vbuff)
{
  if (vbuff->ready) {
    glBindBuffer(GL_ARRAY_BUFFER, vbuff->vbo_id);
    GLvoid *data = vbuff->data;
    if (vbuff->vbo_id) {
      glBufferData(GL_ARRAY_BUFFER, GPU_vertex_format_stride(vbuff->vformat) * vbuff->alloc_count, vbuff->data, vbuff->usage);
      data = 0;
    }

    GPU_vertex_format_bind(vbuff->vformat, data);
  }
  else {
    printf("ERROR: vertex buffer not ready for binding\n");
  }
}
