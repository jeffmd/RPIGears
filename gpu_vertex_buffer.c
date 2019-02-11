// gpu_vertex_buffer.c

#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"
#include "fp16.h"
#include "gpu_shader_interface.h"

#define  VERT_ATTRIB_MAX 8

typedef struct {
  GLenum type;             // GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_FIXED, or GL_FLOAT
  uint8_t size;            // Components per element (1,2,3,4)
  uint8_t normalized;      // Fixed-point values are normalized when converted to floats
  const char *name;        // attribute name in vertex shader
  GLuint offset;           // offset from start of data
  void *data;     // pointer to current location in data buffer for writing
} VertAttribute;

typedef struct {
  uint8_t active;           // not zero if vertex buffer is not deleted
  VertAttribute vertex_attributes[VERT_ATTRIB_MAX];
  uint8_t attribute_count;  // count of active attributes in vertex_attributes array
  uint8_t stride;           // size in bytes of vertex data - only valid after attributes added and ready is not zero
  GLuint draw_count;        // number of verts to draw
  GLuint max_count;         // max number of verts in data buffer
  void *data;               // pointer to data buffer in cpu ram
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
  vbuff->attribute_count = 0;
  vbuff->draw_count = 0;
  vbuff->max_count = 0;
  vbuff->data = 0;
  vbuff->vbo_id = 0;
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


// add attribute to vertbuffer
void GPU_vertbuf_add_attribute(const GLuint vbuff_id, const char *name, const GLint size, const GLenum type,
                             const GLboolean normalized)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];
  VertAttribute *vattr = &vbuff->vertex_attributes[vbuff->attribute_count];
  vbuff->attribute_count++;
  
  vattr->type = type;
  vattr->size = size;
  vattr->normalized = GL_FALSE;
  vattr->name = name;
                               
}

static GLuint get_type_byte_size(const GLenum type)
{
  switch(type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
      return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
      return 2;
    default:
      return 4;
  }
}

// begin data update ( vertex max count ) - no more attributes can be added
void GPU_vertbuf_begin_update(const GLuint vbuff_id, const GLuint max_count)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];

  vbuff->stride = 0;
  GLuint max_Idx = vbuff->attribute_count;
  // calculate stride
  for (GLuint Idx = 0; Idx < max_Idx; Idx++) {
    vbuff->vertex_attributes[Idx].offset = vbuff->stride;
    vbuff->stride += get_type_byte_size(vbuff->vertex_attributes[Idx].type) * vbuff->vertex_attributes[Idx].size;
  }

  if (vbuff->stride > 0) {
    // allocate heap storage for data
    if (!vbuff->data)
      vbuff->data = calloc(max_count, vbuff->stride);
    else if (max_count > vbuff->max_count) {
      vbuff->data = realloc(vbuff->data, max_count * vbuff->stride);
    }

    vbuff->max_count = max_count;

    for (GLuint Idx = 0; Idx < max_Idx; Idx++) {
      vbuff->vertex_attributes[Idx].data = vbuff->data + vbuff->vertex_attributes[Idx].offset;
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
    VertAttribute *vattr = &vbuff->vertex_attributes[attribute_id];
    GLuint size = vattr->size;
    if (vattr->type == GL_FLOAT) {
      GLfloat *data = vattr->data;
      data[0] = val1;
      
      if (size > 1)
        data[1] = val2;
      if (size > 2)
        data[2] = val3;
      if (size > 3)
        data[3] = val4;
    }
    else if (vattr->type == GL_HALF_FLOAT_OES) {
      GLshort *data = vattr->data;
      data[0] = f16(val1);
      if (size > 1)
        data[1] = f16(val2);
      if (size > 2)
        data[2] = f16(val3);
      if (size > 3)
        data[3] = f16(val4);
    }

    vattr->data += vbuff->stride;
  }  
}

void GPU_vertbuf_use_VBO(const GLuint vbuff_id)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];
  
  if (!vbuff->vbo_id) {
    glGenBuffers(1, &vbuff->vbo_id);
  }
}

// set VAO
void GPU_vertbuf_set_VAO(const GLuint vbuff_id)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];

  if (vbuff->ready) {
    glBindBuffer(GL_ARRAY_BUFFER, vbuff->vbo_id);
    GLvoid *data = vbuff->data;
    if (vbuff->vbo_id) {
      glBufferData(GL_ARRAY_BUFFER, vbuff->stride * vbuff->max_count, vbuff->data, vbuff->usage);
      data = 0;
    }

    GLuint max_Idx = vbuff->attribute_count;
    // enable active genaric attributes 
    for (GLuint Idx = 0; Idx < max_Idx; Idx++) {
      VertAttribute *vattr = &vbuff->vertex_attributes[Idx];

      GLuint loc = get_active_attribute_location(vattr->name);
      glEnableVertexAttribArray(loc);
      glVertexAttribPointer(loc, vattr->size, vattr->type, GL_FALSE, vbuff->stride, data + vattr->offset);
    }
  }
}
