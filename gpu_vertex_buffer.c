// gpu_vertex_buffer.c

#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"
#include "gpu_vertex_format.h"

typedef struct {
  uint8_t active;           // not zero if vertex buffer is not deleted
  void *vertex_data[VERT_ATTRIB_MAX];     // pointer to current location in data buffer for writing
  GLuint max_count;         // max number of verts in data buffer
  void *data;               // pointer to data buffer in cpu ram
  uint8_t vformat_id;       // vertex format ID
  GLuint vbo_id;            // 0 indicates using client ram or not allocated yet
  GLenum usage;             // usage hint for GL optimisation
  uint8_t ready;            // not zero if ready for adding data to buffer
} VertBuffer;


#define VERT_BUFFER_MAX_COUNT 10

static VertBuffer vert_buffers[VERT_BUFFER_MAX_COUNT];
static uint16_t next_deleted_vert_buffer = 0;

static GLuint find_deleted_vert_buffer(void)
{
  GLuint id = next_deleted_vert_buffer;
  
  if((id == 0) | (id >= VERT_BUFFER_MAX_COUNT))
    id = 1;

  for ( ; id < VERT_BUFFER_MAX_COUNT; id++) {
    if (vert_buffers[id].active == 0) {
      next_deleted_vert_buffer = id + 1;
      break;
    }
  }

  if (id == VERT_BUFFER_MAX_COUNT) {
    printf("WARNING: No vertex buffers available\n");
    id = 0;
  }

  return id;
}

void GPU_vertbuf_init(const GLuint vbuff_id)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];
  
  vbuff->active = 1;
  vbuff->max_count = 0;
  vbuff->data = 0;
  vbuff->vbo_id = 0;
  vbuff->vformat_id = 0;
	vbuff->usage = GL_STATIC_DRAW;
	vbuff->ready = 0;
  
}

// create new vertbuffer
GLuint GPU_vertbuf_create(void)
{
  const GLuint vbuff_id = find_deleted_vert_buffer();
	GPU_vertbuf_init(vbuff_id);
  printf("New vertbuf ID:%i\n", vbuff_id);

	return vbuff_id;
}

void GPU_vertbuf_delete(const GLuint vbuff_id)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];

  if (vbuff->data) {
    free(vbuff->data);
    vbuff->data = 0;
  }
  
  if (vbuff->vbo_id) {
    glDeleteBuffers(1, &vbuff->vbo_id);
    vbuff->vbo_id = 0;
  }
  
  vbuff->active = 0;
  
  if (vbuff_id < next_deleted_vert_buffer)
    next_deleted_vert_buffer = vbuff_id; 
}

void GPU_vertbuf_set_vertex_format(const GLuint vbuff_id, const GLuint vformat_id)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];

  vbuff->vformat_id = vformat_id;  
}

// begin data update ( vertex max count ) - no more attributes can be added
void GPU_vertbuf_begin_update(const GLuint vbuff_id, const GLuint max_count)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];

  const uint8_t stride = GPU_vertex_format_stride(vbuff->vformat_id);

  if (stride > 0) {
    // allocate heap storage for data
    if (!vbuff->data)
      vbuff->data = calloc(max_count, stride);
    else if (max_count > vbuff->max_count) {
      vbuff->data = realloc(vbuff->data, max_count * stride);
    }

    vbuff->max_count = max_count;
    
    const GLuint max_Idx = GPU_vertex_format_attribute_count(vbuff->vformat_id);
    for (GLuint Idx = 0; Idx < max_Idx; Idx++) {
      vbuff->vertex_data[Idx] = vbuff->data + GPU_vertex_format_offset(vbuff->vformat_id, Idx);
    }

    // ready to receive data updates in buffer storage
    vbuff->ready = 1;
  }
  
}

// add vertex attribute data ( attribute_id, float, float, float )
void GPU_vertbuf_add_4(const GLuint vbuff_id, const GLuint attribute_id, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];

  if (vbuff->ready) {
    GPU_vertex_format_add_4(vbuff->vformat_id, attribute_id, vbuff->vertex_data[attribute_id], val1, val2, val3, val4);
    vbuff->vertex_data[attribute_id] += GPU_vertex_format_stride(vbuff->vformat_id);
  }  
}

void GPU_vertbuf_use_VBO(const GLuint vbuff_id)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];
  
  if (!vbuff->vbo_id) {
    glGenBuffers(1, &vbuff->vbo_id);
  }
}

void GPU_vertbuf_bind(const GLuint vbuff_id)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];

  if (vbuff->ready) {
    glBindBuffer(GL_ARRAY_BUFFER, vbuff->vbo_id);
    GLvoid *data = vbuff->data;
    if (vbuff->vbo_id) {
      glBufferData(GL_ARRAY_BUFFER, GPU_vertex_format_stride(vbuff->vformat_id) * vbuff->max_count, vbuff->data, vbuff->usage);
      data = 0;
    }

    GPU_vertex_format_bind(vbuff->vformat_id, data);
  }
}
