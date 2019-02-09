// gpu_vertex_buffer.c

#include <stdio.h>

#include "gles3.h"

#define  VERT_ATTRIB_MAX 8

typedef struct {
  GLenum type;             // GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_FIXED, or GL_FLOAT
  uint8_t size;            // Components per element (1,2,3,4)
  uint8_t normalized;      // Fixed-point values are normalized when converted to floats
  const char *name;        // attribute name in vertex shader
  unsigned char *data;     // pointer to current location in data buffer for writing
} VertAttribute;

typedef struct {
  uint8_t active;           // not zero if vertex buffer is not deleted
  VertAttribute vertex_attributes[VERT_ATTRIB_MAX];
  uint8_t attribute_count;  // count of active attributes in vertex_attributes array
  uint8_t stride;           // size in bytes of vertex data - only valid after attributes added and ready is not zero
  GLuint draw_count;        // number of verts to draw
  GLuint max_count;         // max number of verts in data buffer
  unsigned char *data;      // pointer to data buffer in cpu ram
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


// add attribute to vertbuffer
void GPU_vertbuf_add_attribute(const GLuint vbuff_id, const char *name, const GLint size, const GLenum type,
                             const GLboolean normalized)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];
  VertAttribute *vattr =&vbuff->vertex_attributes[vbuff->attribute_count];
  vbuff->attribute_count++;
  
  vattr->type = type;
  vattr->size = size;
  vattr->normalized = GL_FALSE;
  vattr->name = name;
                               
}

// begin data update ( vertex max count ) - no more attributes can be added
void GPU_vertbuf_begin_update(const GLuint vbuff_id, const GLuint max_count)
{
  VertBuffer *vbuff = &vert_buffers[vbuff_id];
  vbuff->max_count = max_count;
  
}

// get vertex data size

// add vertex attribute data ( attribute_id, float, float, float )

// add vertex attribute data ( attribute_id, float, float );

// add vertex attribute data ( attribute_id, float );

// set VAO

