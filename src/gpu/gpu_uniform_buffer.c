// gpu_uniform_buffer.c


#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"
#include "id_plug.h"
#include "static_array.h"
#include "gpu_shader.h"

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

typedef struct {
  uint8_t modid;
  ID_t shader;             // the shader used for binding
  GLint locations[UNIFORM_MAX_COUNT];  // uniform bind location
} ShaderUniformIndex;

#define SHADER_CACHE_MAX_COUNT 5

typedef struct {
  uint8_t active;           // not zero if uniform buffer is not deleted
  uint8_t uniform_count;    // count of active attributes in vertex_attributes array
  uint8_t next_index;       // next index to use for a new shader binding
  uint8_t active_index;     // the index to the shader used for current binding
  UniformAttribute uniforms[UNIFORM_MAX_COUNT];
  ShaderUniformIndex shaders[SHADER_CACHE_MAX_COUNT];
} GPUUniformBuffer;

#define UNIFORM_BUFFER_MAX_COUNT 10

static GPUUniformBuffer uniform_buffers[UNIFORM_BUFFER_MAX_COUNT];
static ID_t next_deleted_uniform_buffer;

static ID_t active_uniform_buffer = 0;

static inline ID_t find_deleted_uniform_buffer_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_uniform_buffer, uniform_buffers,
                            UNIFORM_BUFFER_MAX_COUNT, GPUUniformBuffer, "uniform buffer");
}

static GPUUniformBuffer *get_uniform_buffer(ID_t id)
{
  if ((id < 0) | (id >= UNIFORM_BUFFER_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Uniform buffer id, using default id: 0\n");
  }
    
  return uniform_buffers + id;
}

static int uniformbuffer_next_index(GPUUniformBuffer *ubuff)
{
  if (ubuff->next_index >= SHADER_CACHE_MAX_COUNT)
    ubuff->next_index = 0;
      
  return ubuff->next_index++;
}

static int uniformbuffer_needs_binding(GPUUniformBuffer *ubuff, ID_t shader)
{
  int index = 0;
  int needs_binding = 1;
  uint8_t modid = GPU_shader_modid(shader);
  
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

ID_t GPU_uniformbuffer_create(void)
{
  const ID_t id = find_deleted_uniform_buffer_id();
  GPUUniformBuffer *ubuff = get_uniform_buffer(id);
  
  ubuff->active = 1;
  uniformbuffer_init(ubuff);

  return id;
}

void GPU_uniformbuffer_delete(const ID_t id)
{
  GPUUniformBuffer *const ubuff = get_uniform_buffer(id);
  ubuff->active = 0;
  uniformbuffer_init(ubuff);
  
  if (id < next_deleted_uniform_buffer)
    next_deleted_uniform_buffer = id; 

}

void GPU_uniformbuffer_add(const ID_t id, const char *name,
  const GLint size, const GLenum type, void *data)
{
  if (data) {
    GPUUniformBuffer *ubuff = get_uniform_buffer(id);
    UniformAttribute *const uniform = &ubuff->uniforms[ubuff->uniform_count];
    
    uniform->type = type;
    uniform->size = size;
    uniform->name = name;
    uniform->data = data;
    printf("user add uniform name: %s, size: %i\n", name, size);
    // force rebinding to active shader
    uniformbuffer_unbind(ubuff);                             
    ubuff->uniform_count++;
  }
  else {
    printf("ERROR: can't add %s uniform to uniform buffer, buffer pointer is NULL\n", name);
  }
}

void GPU_uniformbuffer_bind(const ID_t id)
{
  int shader = GPU_shader_active_shader();
  GPUUniformBuffer *const ubuff = get_uniform_buffer(id);
  
  if ((ubuff->shaders[ubuff->active_index].shader != shader)
    || (ubuff->shaders[ubuff->active_index].modid != GPU_shader_modid(shader))) {
    if (uniformbuffer_needs_binding(ubuff, shader)) {
      uniformbuffer_update_locations(ubuff);
    }
  }
}

void GPU_uniformbuffer_update(const ID_t id)
{
  GPU_uniformbuffer_bind(id);
  
  GPUUniformBuffer *const ubuff = get_uniform_buffer(id);
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

void GPU_uniformbuffer_activate(const ID_t id)
{
  active_uniform_buffer = id;
}

void GPU_uniformbuffer_update_active(void)
{
  if (active_uniform_buffer)
    GPU_uniformbuffer_update(active_uniform_buffer);
}
