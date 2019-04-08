// gpu_uniform_buffer.c


#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"
#include "gpu_shader.h"

#define UNIFORM_MAX_COUNT 8

typedef struct {
  GLint location;          // uniform bind location
  GLenum type;             // storage type in buffer: GL_FLOAT, GL_FLOAT_VEC2,
                           // GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_INT, GL_INT_VEC2,
                           // GL_INT_VEC3, GL_INT_VEC4, GL_BOOL, GL_BOOL_VEC2,
                           // GL_BOOL_VEC3, GL_BOOL_VEC4, GL_FLOAT_MAT2,
                           // GL_FLOAT_MAT3, GL_FLOAT_MAT4, GL_SAMPLER_2D, or GL_SAMPLER_CUBE
  uint8_t size;            // number of components of type that make up uniform
  const char *name;        // uniform name in shader
  //GLuint stride;           // number of bytes to next instance of uniform
  void *data;              // pointer to current location in data buffer for writing/reading
} GPUUniformAttribute;

typedef struct {
  uint8_t active;           // not zero if uniform buffer is not deleted
  GPUUniformAttribute uniforms[UNIFORM_MAX_COUNT];
  uint8_t uniform_count;    // count of active attributes in vertex_attributes array
  GPUShader *shader;        // the shader used for binding
} GPUUniformBuffer;

#define UNIFORM_BUFFER_MAX_COUNT 10

static GPUUniformBuffer uniform_buffers[UNIFORM_BUFFER_MAX_COUNT];
static GPUUniformBuffer *next_deleted_uniform_buffer = 0;

static GPUUniformBuffer *find_deleted_uniform_buffer(void)
{
  GPUUniformBuffer *ubuff = next_deleted_uniform_buffer;
  const GPUUniformBuffer *max_ubuff = uniform_buffers + UNIFORM_BUFFER_MAX_COUNT;

  if((ubuff <= uniform_buffers) | (ubuff >= max_ubuff))
    ubuff = uniform_buffers + 1;

  for ( ; ubuff < max_ubuff; ubuff++) {
    if (ubuff->active == 0) {
      next_deleted_uniform_buffer = ubuff + 1;
      break;
    }
  }

  if (ubuff == max_ubuff) {
    printf("WARNING: No uniform buffers available\n");
    ubuff = uniform_buffers;
  }

  return ubuff;
}

void GPU_uniformbuffer_init(GPUUniformBuffer *ubuff)
{
  ubuff->active = 1;
  ubuff->uniform_count = 0;
  ubuff->shader = 0;

}


GPUUniformBuffer *GPU_uniformbuffer_create(void)
{
  GPUUniformBuffer *ubuff = find_deleted_uniform_buffer();
  GPU_uniformbuffer_init(ubuff);
  printf("New uniform buffer ID:%p\n", ubuff);

  return ubuff;
}

void GPU_uniformbuffer_delete(GPUUniformBuffer *ubuff)
{
  ubuff->active = 0;
  
  if (ubuff < next_deleted_uniform_buffer)
    next_deleted_uniform_buffer = ubuff; 

}

void GPU_uniformbuffer_add_uniform(GPUUniformBuffer *ubuff, const char *name,
  const GLint size, const GLenum type, void *data)
{
  GPUUniformAttribute *uniform = &ubuff->uniforms[ubuff->uniform_count];
  
  uniform->type = type;
  uniform->size = size;
  uniform->name = name;
  uniform->data = data;
  uniform->location = -1;
  
  // force rebinding to active shader
  ubuff->shader = 0;                             
  ubuff->uniform_count++;

}

void GPU_uniformbuffer_bind(GPUUniformBuffer *ubuff)
{
  GPUShader *shader = GPU_shader_get_active();
  if (ubuff->shader != shader) {
    ubuff->shader = shader;
    for (int idx=0; idx < ubuff->uniform_count; idx++) {
      GPUUniformAttribute *uniform = &ubuff->uniforms[idx];
      uniform->location = GPU_get_active_uniform_location(uniform->name);
    }
  }
}

void GPU_uniformbuffer_update(GPUUniformBuffer *ubuff)
{
  GPU_uniformbuffer_bind(ubuff);
  
  for (int idx = 0; idx < ubuff->uniform_count; idx++) {
    GPUUniformAttribute *uniform = &ubuff->uniforms[idx];
    if (uniform->location >= 0) {
      switch(uniform->type) {
        case GL_FLOAT:
          glUniform1fv(uniform->location, uniform->size, uniform->data);
          break;
        case GL_FLOAT_VEC2:
          glUniform2fv(uniform->location, uniform->size, uniform->data);
          break;
        case GL_FLOAT_VEC3:
          glUniform3fv(uniform->location, uniform->size, uniform->data);
          break;
        case GL_FLOAT_VEC4:
          glUniform4fv(uniform->location, uniform->size, uniform->data);
          break;
        case GL_SAMPLER_2D:
        case GL_INT:
          glUniform1iv(uniform->location, uniform->size, uniform->data);
          break;
        case GL_INT_VEC2:
          glUniform2iv(uniform->location, uniform->size, uniform->data);
          break;
        case GL_INT_VEC3:
          glUniform3iv(uniform->location, uniform->size, uniform->data);
          break;
        case GL_INT_VEC4:
          glUniform4iv(uniform->location, uniform->size, uniform->data);
          break;
        case GL_FLOAT_MAT4:
          glUniformMatrix4fv(uniform->location, uniform->size, GL_FALSE, uniform->data);
          break;
          
      }
    }
  }
}

