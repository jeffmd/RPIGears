// gpu_shader_interface.c

#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "shaderid.h"

#define NAME_BUFFER_SIZE 100
#define SHADER_INTERFACES_MAX_COUNT 5
#define SHADER_INPUTS_MAX_COUNT SHADER_INTERFACES_MAX_COUNT * 10
#define INPUT_NAME_LENGTH 16

extern char msg[512];

//static char nameBuffer[NAME_BUFFER_SIZE];
//static GLuint nameCount = 0;

typedef struct {
  //GLuint name_offset;
  char name[INPUT_NAME_LENGTH];
  GLint location;
  GLint size;
  GLenum type;
} GPUShaderInput;

typedef struct {
  GLint count;
  GLint start;
  GLint name_max_length;
//  GLint name_start;
} ShaderInputArrayTracker;

typedef struct {
  GLuint glProgramObj;
  ShaderInputArrayTracker uniform_array;
  ShaderInputArrayTracker attribute_array;
} GPUShaderInterface;

static GPUShaderInterface shaderInterfaces[SHADER_INTERFACES_MAX_COUNT];

static GPUShaderInput shader_inputs[SHADER_INPUTS_MAX_COUNT];
static unsigned int shader_inputs_count = 0;

static GLuint active_programID = 0;

GLuint get_shader_program_obj(const PROGRAM_ID_T programID)
{
  return shaderInterfaces[programID].glProgramObj;
}

GLuint init_shader_interface(const PROGRAM_ID_T programID)
{
  GPUShaderInterface *prg_interface = &shaderInterfaces[programID];
   /* Create and attach shaders to program */
  prg_interface->glProgramObj = glCreateProgram();
  printf("program object: %u\n", prg_interface->glProgramObj);

  return prg_interface->glProgramObj;
}

static void update_array_tracker(const GLuint programID, const GLboolean is_uniform)
{
  GPUShaderInterface *prg_interface = &shaderInterfaces[programID];
  ShaderInputArrayTracker *tracker = is_uniform ? &prg_interface->uniform_array : &prg_interface->attribute_array;
  GLint old_count = tracker->count;

  glGetProgramiv(prg_interface->glProgramObj, is_uniform ? GL_ACTIVE_UNIFORMS : GL_ACTIVE_ATTRIBUTES, &tracker->count);
  glGetProgramiv(prg_interface->glProgramObj, is_uniform ? GL_ACTIVE_UNIFORM_MAX_LENGTH : GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &tracker->name_max_length);
  
  if (tracker->count > old_count) {
    tracker->start = shader_inputs_count;
    shader_inputs_count += tracker->count;
  }
  printf("%s count: %i\n", is_uniform ? "uniform" : "attribute", tracker->count);
}

static void build_input_list(const GLuint programID, const GLboolean is_uniform)
{
  GPUShaderInterface *prg_interface = &shaderInterfaces[programID];
  ShaderInputArrayTracker *tracker = is_uniform ? &prg_interface->uniform_array : &prg_interface->attribute_array;
  
 
  const GLuint end = tracker->count;
	for (GLuint i = 0; i < end; ++i) {
    GPUShaderInput *input = &shader_inputs[tracker->start + i];
		is_uniform ? glGetActiveUniform(prg_interface->glProgramObj, i, INPUT_NAME_LENGTH, NULL, &input->size, &input->type, input->name) :
                 glGetActiveAttrib(prg_interface->glProgramObj, i, INPUT_NAME_LENGTH, NULL, &input->size, &input->type, input->name);

		input->location = is_uniform ? glGetUniformLocation(prg_interface->glProgramObj, input->name) :
                                   glGetAttribLocation(prg_interface->glProgramObj, input->name);
    printf("added %s: %s, location: %i\n", is_uniform ? "uniform" : "attribute", input->name, input->location);
  }
}

static void build_uniform_list(const GLuint programID)
{
  update_array_tracker(programID, GL_TRUE);
  build_input_list(programID, GL_TRUE);
}

static void build_attribute_list(const GLuint programID)
{
  update_array_tracker(programID, GL_FALSE);
  build_input_list(programID, GL_FALSE);
}

void enable_shader_program(const PROGRAM_ID_T programID)
{
  const GLuint program = get_shader_program_obj(programID); 

  glLinkProgram(program);
  glGetProgramInfoLog(program, sizeof msg, NULL, msg);
  printf("Link info: %s\n", msg);

  build_uniform_list(programID);
  build_attribute_list(programID);
  
  /* Enable the shaders */
  glUseProgram(program);
  active_programID = programID;

}

static GPUShaderInput *get_shader_input(const ShaderInputArrayTracker* input_tracker, const char *name)
{
  const GLint end = input_tracker->start + input_tracker->count;
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

GLint get_active_uniform_location(const char *name)
{
  return get_shader_input_location(&shaderInterfaces[active_programID].uniform_array, name);
}

GLint get_active_attribute_location(const char *name)
{
  return get_shader_input_location(&shaderInterfaces[active_programID].attribute_array, name);
}

