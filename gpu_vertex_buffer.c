// gpu_vertex_buffer.c

#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"
#include "gpu_vertex_format.h"
#include "static_array.h"

typedef struct {
  uint8_t active;           // not zero if vertex buffer is not deleted
  void *vertex_data[VERT_ATTRIB_MAX];     // pointer to current location in data buffer for writing
  GLuint max_count;         // max number of verts in data buffer
  void *data;               // pointer to data buffer in cpu ram
  GPUVertFormat *vformat;      // vertex format ID
  GLuint vbo_id;            // 0 indicates using client ram or not allocated yet
  GLenum usage;             // usage hint for GL optimisation
  uint8_t ready;            // not zero if ready for adding data to buffer
} GPUVertBuffer;

#define VERT_BUFFER_MAX_COUNT 10

static GPUVertBuffer vert_buffers[VERT_BUFFER_MAX_COUNT];
static GPUVertBuffer *next_deleted_vert_buffer = 0;

static inline GPUVertBuffer *find_deleted_vert_buffer(void)
{
  return ARRAY_FIND_DELETED(next_deleted_vert_buffer, vert_buffers,
                            VERT_BUFFER_MAX_COUNT, "vertex buffer");
}

static void vertbuf_init(GPUVertBuffer *vbuff)
{
  vbuff->max_count = 0;
  vbuff->data = 0;
  vbuff->vbo_id = 0;
  vbuff->vformat = 0;
  vbuff->usage = GL_STATIC_DRAW;
  vbuff->ready = 0;

}

// create new GPUVertBuffer
GPUVertBuffer *GPU_vertbuf_create(void)
{
  GPUVertBuffer *vbuff = find_deleted_vert_buffer();
  vbuff->active = 1;
  vertbuf_init(vbuff);
  printf("New vertbuf ID:%p\n", vbuff);

  return vbuff;
}

void GPU_vertbuf_delete(GPUVertBuffer *vbuff)
{
  if (vbuff->data) {
    free(vbuff->data);
  }

  if (vbuff->vbo_id) {
    glDeleteBuffers(1, &vbuff->vbo_id);
  }

  vbuff->active = 0;
  vertbuf_init(vbuff);

  if (vbuff < next_deleted_vert_buffer)
    next_deleted_vert_buffer = vbuff;
}

void GPU_vertbuf_set_vertex_format(GPUVertBuffer *vbuff, GPUVertFormat *vformat)
{
  vbuff->vformat = vformat;
}

// begin data update ( vertex max count ) - no more attributes can be added
void GPU_vertbuf_begin_update(GPUVertBuffer *vbuff, const GLuint max_count)
{
  const uint8_t stride = GPU_vertex_format_stride(vbuff->vformat);

  if (stride > 0) {
    // allocate heap storage for data
    if (!vbuff->data)
      vbuff->data = calloc(max_count, stride);
    else if (max_count > vbuff->max_count) {
      vbuff->data = realloc(vbuff->data, max_count * stride);
    }

    vbuff->max_count = max_count;

    const GLuint max_Idx = GPU_vertex_format_attribute_count(vbuff->vformat);
    for (GLuint Idx = 0; Idx < max_Idx; Idx++) {
      vbuff->vertex_data[Idx] = vbuff->data + GPU_vertex_format_offset(vbuff->vformat, Idx);
    }

    // ready to receive data updates in buffer storage
    vbuff->ready = 1;
  }

}

// add vertex attribute data ( attribute_id, float, float, float )
void GPU_vertbuf_add_4(GPUVertBuffer *vbuff, const GLuint attribute_id, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4)
{
  if (vbuff->ready) {
    GPU_vertex_format_add_4(vbuff->vformat, attribute_id, vbuff->vertex_data[attribute_id], val1, val2, val3, val4);
    vbuff->vertex_data[attribute_id] += GPU_vertex_format_stride(vbuff->vformat);
  }
}

void GPU_vertbuf_use_VBO(GPUVertBuffer *vbuff)
{
  if (!vbuff->vbo_id) {
    glGenBuffers(1, &vbuff->vbo_id);
  }
}

void GPU_vertbuf_bind(GPUVertBuffer *vbuff)
{
  if (vbuff->ready) {
    glBindBuffer(GL_ARRAY_BUFFER, vbuff->vbo_id);
    GLvoid *data = vbuff->data;
    if (vbuff->vbo_id) {
      glBufferData(GL_ARRAY_BUFFER, GPU_vertex_format_stride(vbuff->vformat) * vbuff->max_count, vbuff->data, vbuff->usage);
      data = 0;
    }

    GPU_vertex_format_bind(vbuff->vformat, data);
  }
}
