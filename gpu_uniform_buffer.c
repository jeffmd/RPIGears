// gpu_uniform_buffer.c


#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"

#define UNIFORM_ATTRIB_MAX 8

typedef struct {
  GLenum type;             // storage type in buffer: GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, or GL_FLOAT
  uint8_t size;            // number of components of type that make up uniform
  const char *name;        // uniform name in shader
  GLuint stride;           // number of bytes to next instance of uniform
  void *data;              // pointer to current location in data buffer for writing/reading
} UniformAttribute;

typedef struct {
  uint8_t active;           // not zero if uniform buffer is not deleted
  UniformAttribute uniform_attributes[UNIFORM_ATTRIB_MAX];
  uint8_t uniform_count;    // count of active attributes in vertex_attributes array
} UniformBuffer;

#define UNIFORM_BUFFER_MAX_COUNT 10
