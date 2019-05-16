// gpu_uniform_buffer.c


#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"
#include "gpu_shader.h"
#include "static_array.h"

#define UNIFORM_MAX_COUNT 8

typedef struct {
  GLenum type;             // storage type in buffer: GL_FLOAT, GL_FLOAT_VEC2,
                           // GL_FLOAT_VEC3, GL_FLOAT_VEC4, GL_INT, GL_INT_VEC2,
                           // GL_INT_VEC3, GL_INT_VEC4, GL_BOOL, GL_BOOL_VEC2,
                           // GL_BOOL_VEC3, GL_BOOL_VEC4, GL_FLOAT_MAT2,
                           // GL_FLOAT_MAT3, GL_FLOAT_MAT4, GL_SAMPLER_2D, or GL_SAMPLER_CUBE
  uint8_t size;            // number of components of type that make up uniform
  const char *name;        // uniform name in shader
  //GLuint stride;           // number of bytes to next instance of uniform
  void *data;              // pointer to current location in data buffer for writing/reading
} UniformAttribute;

typedef struct ShaderUniformIndex{
  GPUShader *shader;        // the shader used for binding
  int modid;
  GLint locations[UNIFORM_MAX_COUNT];  // uniform bind location
} ShaderUniformIndex;

#define SHADER_CACHE_MAX_COUNT 5

typedef struct {
  uint8_t active;           // not zero if uniform buffer is not deleted
  UniformAttribute uniforms[UNIFORM_MAX_COUNT];
  uint8_t uniform_count;    // count of active attributes in vertex_attributes array
  ShaderUniformIndex shaders[SHADER_CACHE_MAX_COUNT];
  uint8_t next_index;       // next index to use for a new shader binding
  uint8_t active_index;     // the index to the shader used for current binding
} GPUUniformBuffer;

#define UNIFORM_BUFFER_MAX_COUNT 10

static GPUUniformBuffer uniform_buffers[UNIFORM_BUFFER_MAX_COUNT];
static GPUUniformBuffer *next_deleted_uniform_buffer = 0;


static inline GPUUniformBuffer *find_deleted_uniform_buffer(void)
{
  return ARRAY_FIND_DELETED(next_deleted_uniform_buffer, uniform_buffers,
                            UNIFORM_BUFFER_MAX_COUNT, "uniform buffer");
}

static int uniformbuffer_next_index(GPUUniformBuffer *ubuff)
{
  if (ubuff->next_index >= SHADER_CACHE_MAX_COUNT)
    ubuff->next_index = 0;
      
  return ubuff->next_index++;
}

static int uniformbuffer_needs_binding(GPUUniformBuffer *ubuff, GPUShader *shader)
{
  int index = 0;
  int needs_binding = 1;
  int modid = GPU_shader_modid(shader);
  
  for ( ; index < SHADER_CACHE_MAX_COUNT; index++) {
    if (ubuff->shaders[index].shader == shader) {
      if (ubuff->shaders[index].modid == modid)
        needs_binding = 0; // no changes in shader
      break;
    }
    
    if (ubuff->shaders[index].shader == 0) {
      break;
    }
  }
  
  if (index >= SHADER_CACHE_MAX_COUNT) {
    index = uniformbuffer_next_index(ubuff);
  }
  
  ubuff->shaders[index].shader = shader;
  ubuff->shaders[index].modid = modid;
  ubuff->active_index = index;
  
  return needs_binding;
}

static void uniformbuffer_unbind(GPUUniformBuffer *ubuff)
{
  ubuff->shaders[ubuff->active_index].shader = 0;
}

static void uniformbuffer_init(GPUUniformBuffer *ubuff)
{
  ubuff->uniform_count = 0;
  ubuff->active_index = 0;
  ubuff->next_index = 1;
  
  uniformbuffer_unbind(ubuff);
}

static void uniformbuffer_update_locations(GPUUniformBuffer *ubuff)
{
  GLint *locations = ubuff->shaders[ubuff->active_index].locations;
  
  for (int idx = 0; idx < ubuff->uniform_count; idx++) {
    locations[idx] = GPU_get_active_uniform_location(ubuff->uniforms[idx].name);
  }
  
  printf("updating uniform locations\n");
}

GPUUniformBuffer *GPU_uniformbuffer_create(void)
{
  GPUUniformBuffer *ubuff = find_deleted_uniform_buffer();
  ubuff->active = 1;
  uniformbuffer_init(ubuff);

  return ubuff;
}

void GPU_uniformbuffer_delete(GPUUniformBuffer *ubuff)
{
  ubuff->active = 0;
  uniformbuffer_init(ubuff);
  
  if (ubuff < next_deleted_uniform_buffer)
    next_deleted_uniform_buffer = ubuff; 

}

void GPU_uniformbuffer_add_uniform(GPUUniformBuffer *ubuff, const char *name,
  const GLint size, const GLenum type, void *data)
{
  UniformAttribute *uniform = &ubuff->uniforms[ubuff->uniform_count];
  
  uniform->type = type;
  uniform->size = size;
  uniform->name = name;
  uniform->data = data;
  
  // force rebinding to active shader
  uniformbuffer_unbind(ubuff);                             
  ubuff->uniform_count++;

}

void GPU_uniformbuffer_bind(GPUUniformBuffer *ubuff)
{
  GPUShader *shader = GPU_shader_get_active();
  
  if (ubuff->shaders[ubuff->active_index].shader != shader) {
    if (uniformbuffer_needs_binding(ubuff, shader)) {
      uniformbuffer_update_locations(ubuff);
    }
  }
}

void GPU_uniformbuffer_update(GPUUniformBuffer *ubuff)
{
  GPU_uniformbuffer_bind(ubuff);
  
  GLint *locations = ubuff->shaders[ubuff->active_index].locations;

  for (int idx = 0; idx < ubuff->uniform_count; idx++) {
    int location = locations[idx];
    if (location >= 0) {
      UniformAttribute *uniform = &ubuff->uniforms[idx];
  
      switch(uniform->type) {
        case GL_FLOAT:
          glUniform1fv(location, uniform->size, uniform->data);
          break;
        case GL_FLOAT_VEC2:
          glUniform2fv(location, uniform->size, uniform->data);
          break;
        case GL_FLOAT_VEC3:
          glUniform3fv(location, uniform->size, uniform->data);
          break;
        case GL_FLOAT_VEC4:
          glUniform4fv(location, uniform->size, uniform->data);
          break;
        case GL_SAMPLER_2D:
        case GL_INT:
          glUniform1iv(location, uniform->size, uniform->data);
          break;
        case GL_INT_VEC2:
          glUniform2iv(location, uniform->size, uniform->data);
          break;
        case GL_INT_VEC3:
          glUniform3iv(location, uniform->size, uniform->data);
          break;
        case GL_INT_VEC4:
          glUniform4iv(location, uniform->size, uniform->data);
          break;
        case GL_FLOAT_MAT4:
          glUniformMatrix4fv(location, uniform->size, GL_FALSE, uniform->data);
          break;
          
      }
    }
  }
}

