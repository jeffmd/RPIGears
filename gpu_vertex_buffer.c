// gpu_vertex_buffer.c

#include <stdio.h>

#include "gles3.h"

#define  VERT_ATTRIB_MAX 8

typedef struct VertAttribute {
	GLenum type; // GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_FIXED, or GL_FLOAT
	GLint size; /* 1 to 4 */
	const char *name;
} VertAttribute;

typedef struct {
	unsigned char active;
	VertAttribute vertex_attributes[VERT_ATTRIB_MAX];
	uint8_t attribute_count;
	GLuint draw_count;     // number of verts to draw
	GLuint max_count;      // number of verts in data buffer
	unsigned char *data;   // pointer to data buffer in cpu ram
	GLuint vbo_id;         // 0 indicates using client ram or not allocated yet
	GLenum usage;          // usage hint for GL optimisation
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

// create new vertbuffer

// add attribute to vertbuffer

// init data buffer ( vertex max count )

// add vertex attribute data ( attribute_id, float, float, float )

// add vertex attribute data ( attribute_id, float, float );

// add vertex attribute data ( attribute_id, float );
