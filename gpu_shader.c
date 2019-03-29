// gpu_shader.c

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "gpu_shader_unit.h"

#define NAME_BUFFER_SIZE 100
#define SHADER_MAX_COUNT 5
#define SHADER_INPUTS_MAX_COUNT SHADER_MAX_COUNT * 10
#define INPUT_NAME_LENGTH 16

extern char msg[512];

//static char nameBuffer[NAME_BUFFER_SIZE];
//static GLuint nameCount = 0;

typedef struct {
  //GLuint name_offset;
  char name[INPUT_NAME_LENGTH];
  uint8_t location;
  uint8_t size;
  GLenum type;
} GPUShaderInput;

typedef struct {
  uint16_t count; // number of consecutive elements in GPUShaderInput array
  uint16_t start; // start index into GPUShaderInput array
  uint8_t name_max_length;
//  GLint name_start;
} ShaderInputArrayTracker;

typedef struct {
  GPUShaderUnit *vert_unit;
  GPUShaderUnit *frag_unit;
  uint16_t glProgramObj;
  ShaderInputArrayTracker uniform_array;
  ShaderInputArrayTracker attribute_array;
} GPUShader;

static GPUShader shaders[SHADER_MAX_COUNT];
static GPUShader *next_deleted_shader = 0;


static GPUShaderInput shader_inputs[SHADER_INPUTS_MAX_COUNT];
static uint16_t shader_inputs_count = 0;

static GPUShader *active_shader = 0;

static GPUShader *find_deleted_shader(void)
{
  GPUShader *shader = next_deleted_shader;
  GPUShader *max_shader = shaders + SHADER_MAX_COUNT;
  
  if ((shader <= shaders) | (shader >= max_shader)) {
    shader = shaders + 1;
  }
  
  for (; shader < max_shader; shader++) {
    if (shader->vert_unit == 0) {
      next_deleted_shader = shader + 1;
      break;
    }
  }
  
  if (shader == max_shader) {
    printf("WARNING: No shaders available\n");
    shader = shaders;
  }
  
  return shader;
}

void GPU_shader_init(GPUShader *shader)
{
  GPU_shader_unit_init(shader->vert_unit);
  GPU_shader_unit_init(shader->frag_unit);
  
  shader->glProgramObj = glCreateProgram();
  printf("program object: %u\n", shader->glProgramObj);
  
  glAttachShader(shader->glProgramObj, GPU_shader_unit_globj(shader->vert_unit));
  glAttachShader(shader->glProgramObj, GPU_shader_unit_globj(shader->frag_unit));
}

GPUShader *GPU_shader_create(const char *vertex_file_name, const char *fragment_file_name)
{
  GPUShader *shader = find_deleted_shader();
  
  shader->vert_unit = GPU_shader_unit_create(vertex_file_name, GL_VERTEX_SHADER);
  shader->frag_unit = GPU_shader_unit_create(fragment_file_name, GL_FRAGMENT_SHADER);
  
  return shader;
}

void GPU_shader_gldelete(GPUShader *shader)
{
  glUseProgram(0);

  GPU_shader_unit_glDelete(shader->vert_unit);
  GPU_shader_unit_glDelete(shader->frag_unit);
  glDeleteProgram(shader->glProgramObj);
  shader->glProgramObj = 0;
}


static void update_array_tracker(GPUShader *shader, const GLboolean is_uniform)
{
  ShaderInputArrayTracker *tracker = is_uniform ? &shader->uniform_array : &shader->attribute_array;
  GLint old_count = tracker->count;

  GLint val;
  glGetProgramiv(shader->glProgramObj, is_uniform ? GL_ACTIVE_UNIFORMS : GL_ACTIVE_ATTRIBUTES, &val);
  tracker->count = val;
  glGetProgramiv(shader->glProgramObj, is_uniform ? GL_ACTIVE_UNIFORM_MAX_LENGTH : GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &val);
  tracker->name_max_length = val;

  if (tracker->count > old_count) {
    tracker->start = shader_inputs_count;
    shader_inputs_count += tracker->count;
  }
  printf("%s count: %i\n", is_uniform ? "uniform" : "attribute", tracker->count);
}

static void build_input_list(GPUShader *shader, const GLboolean is_uniform)
{
  ShaderInputArrayTracker *tracker = is_uniform ? &shader->uniform_array : &shader->attribute_array;


  const GLuint end = tracker->count;
  for (GLuint i = 0; i < end; ++i) {
    GPUShaderInput *input = &shader_inputs[tracker->start + i];
    GLint size;
    is_uniform ? glGetActiveUniform(shader->glProgramObj, i, INPUT_NAME_LENGTH, NULL, &size, &input->type, input->name) :
                 glGetActiveAttrib(shader->glProgramObj, i, INPUT_NAME_LENGTH, NULL, &size, &input->type, input->name);
    input->size = size;
    input->location = is_uniform ? glGetUniformLocation(shader->glProgramObj, input->name) :
                                   glGetAttribLocation(shader->glProgramObj, input->name);
    printf("added %s: %s, location: %i, array size: %i\n", is_uniform ? "uniform" : "attribute", input->name, input->location, input->size);
  }
}

static void build_uniform_list(GPUShader *shader)
{
  update_array_tracker(shader, GL_TRUE);
  build_input_list(shader, GL_TRUE);
}

static void build_attribute_list(GPUShader *shader)
{
  update_array_tracker(shader, GL_FALSE);
  build_input_list(shader, GL_FALSE);
}

void GPU_shader_bind(GPUShader *shader)
{
  const GLuint program = shader->glProgramObj;

  glLinkProgram(program);
  glGetProgramInfoLog(program, sizeof msg, NULL, msg);
  printf("Link info: %s\n", msg);

  build_uniform_list(shader);
  build_attribute_list(shader);

  /* Enable the shaders */
  glUseProgram(program);
  active_shader = shader;

}

GLuint GPU_shader_glprogram_obj(GPUShader* shader)
{
  return shader->glProgramObj;
}

static GPUShaderInput *get_shader_input(const ShaderInputArrayTracker* input_tracker, const char *name)
{
  const GLuint end = input_tracker->start + input_tracker->count;
  for (GLuint i = input_tracker->start; i < end; ++i) {
    if (strncmp(shader_inputs[i].name, name, INPUT_NAME_LENGTH) == 0) {
      //printf("location: %i\n", shader_inputs[i].location);
      return &shader_inputs[i];
    }
  }
  return NULL;
}

static inline GLint get_shader_input_location(const ShaderInputArrayTracker* input_tracker, const char *name)
{
  const GPUShaderInput *input = get_shader_input(input_tracker, name);
  return (input) ? input->location : -1;
}

GLint GPU_shader_uniform_location(GPUShader *shader, const char *name)
{
  return get_shader_input_location(&shader->uniform_array, name);
}

GLint GPU_shader_attribute_location(GPUShader *shader, const char *name)
{
  return get_shader_input_location(&shader->attribute_array, name);
}

GLint GPU_get_active_uniform_location(const char *name)
{
  return get_shader_input_location(&active_shader->uniform_array, name);
}

GLint GPU_get_active_attribute_location(const char *name)
{
  return get_shader_input_location(&active_shader->attribute_array, name);
}

