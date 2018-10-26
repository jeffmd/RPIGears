/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "shaderid.h"

#define BUFFSIZE 5000
#define NAME_BUFFER_SIZE 100

// temp shader source buffer for gles2
static char shaderBuf[BUFFSIZE];
static char msg[512];
static char nameBuffer[NAME_BUFFER_SIZE];
static GLuint nameCount = 0;

typedef struct {
  //GLuint name_offset;
  char name[16];
  GLint location;
  GLint size;
  GLenum type;
} GPUShaderInput;

typedef struct {
  GLint count;
  GLint start;
  GLint name_max_length;
  GLint name_start;
} ShaderInputArrayTracker;

typedef struct {
  const char *fileName;
  const GLuint type;
  GLuint glShaderObj;
} SHADER_T;

typedef enum {
  BLINN_PHONG_VS,
  BLINN_PHONG_FS
} SHADER_ID_T;

typedef struct {
  const SHADER_ID_T shader_vertID;
  const SHADER_ID_T shader_fragID;
  GLuint glProgramObj;
  ShaderInputArrayTracker uniform_array;
  ShaderInputArrayTracker attribute_array;
} SHADER_PROGRAM_T;


SHADER_T shaders[] = {
  [BLINN_PHONG_VS] = {"blinn_phong_vert.glsl", GL_VERTEX_SHADER, 0},
  [BLINN_PHONG_FS] = {"blinn_phong_frag.glsl", GL_FRAGMENT_SHADER, 0}
};

SHADER_PROGRAM_T shaderPrograms[] = {
  [BLINN_PHONG_PRG] = {BLINN_PHONG_VS, BLINN_PHONG_FS, 0}
};

static GPUShaderInput shader_inputs[50];
static unsigned int shader_inputs_count = 0;
static GLuint active_programID = 0;

static inline GLuint get_shader_obj(SHADER_ID_T shaderID)
{
  return shaders[shaderID].glShaderObj;
}

static inline GLuint get_shader_program_obj(PROGRAM_ID_T programID)
{
  return shaderPrograms[programID].glProgramObj;
}

static inline GLuint get_shader_program_vert_obj(PROGRAM_ID_T programID)
{
  return get_shader_obj(shaderPrograms[programID].shader_vertID);
}

static inline GLuint get_shader_program_frag_obj(PROGRAM_ID_T programID)
{
  return get_shader_obj(shaderPrograms[programID].shader_fragID);
}

// load shader from a file
static int load_shaderBuf_file(const char *name)
{
  FILE *fp;
  int result = 0;

  strcpy(msg, "shaders/");
  strcat(msg, name);

  fp = fopen(msg, "r");
  if (fp) {
    result = fread(shaderBuf, sizeof(char), BUFFSIZE, fp);
    shaderBuf[result - 1] = 0;
    fclose(fp);
    printf("shader %s loaded\n", name);
  }
  else {
    printf("failed to load %s\n", name);
  }

  return result;
}


static void init_shader(const SHADER_ID_T shaderID)
{
  SHADER_T *shader = &shaders[shaderID];
  const GLchar *src = shaderBuf;
  
  shader->glShaderObj = glCreateShader(shader->type);

  load_shaderBuf_file(shader->fileName);
  glShaderSource(shader->glShaderObj, 1, &src, NULL);
  glCompileShader(shader->glShaderObj);
  
  glGetShaderInfoLog(shader->glShaderObj, sizeof msg, NULL, msg);

  const char * typeStr = (shader->type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
  printf("%s shader object: %u\n", typeStr, shader->glShaderObj);
  printf("%s shader Compile info: %s\n", typeStr, msg);

}

static void init_shader_program(const PROGRAM_ID_T programID)
{
  SHADER_PROGRAM_T *program = &shaderPrograms[programID];
  
  init_shader(program->shader_vertID);
  init_shader(program->shader_fragID);
  
   /* Create and attach shaders to program */
  program->glProgramObj = glCreateProgram();
  printf("program object: %u\n", program->glProgramObj);
  glAttachShader(program->glProgramObj, get_shader_obj(program->shader_vertID));
  glAttachShader(program->glProgramObj, get_shader_obj(program->shader_fragID));
}

static void load_shaders(void)
{
  printf("loading shaders...\n");
  init_shader_program(BLINN_PHONG_PRG);
}

static void delete_shader_program(PROGRAM_ID_T programID)
{
  glUseProgram(0);

  const SHADER_PROGRAM_T *program = &shaderPrograms[programID];

  glDeleteShader(get_shader_obj(program->shader_vertID));
  glDeleteShader(get_shader_obj(program->shader_fragID));
  glDeleteProgram(program->glProgramObj);
}

void delete_shader_programs(void)
{
  delete_shader_program(BLINN_PHONG_PRG);
}

static void build_uniform_list(const GLuint programID)
{
  SHADER_PROGRAM_T *program = &shaderPrograms[programID];
  GLint old_uniform_count = program->uniform_array.count;
  
  glGetProgramiv(program->glProgramObj, GL_ACTIVE_UNIFORMS, &program->uniform_array.count);
  printf("uniform count: %i\n", program->uniform_array.count);

  glGetProgramiv(program->glProgramObj, GL_ACTIVE_UNIFORM_MAX_LENGTH, &program->uniform_array.name_max_length);
  
  if (program->uniform_array.count > old_uniform_count) {
    program->uniform_array.start = shader_inputs_count;
    shader_inputs_count += program->uniform_array.count;
  }
  
  const GLuint end = program->uniform_array.count;
	for (GLuint i = 0; i < end; ++i) {
    GPUShaderInput *input = &shader_inputs[program->uniform_array.start + i];
		glGetActiveUniform(program->glProgramObj, i, 16, NULL, &input->size, &input->type, input->name);

		input->location = glGetUniformLocation(program->glProgramObj, input->name);
    printf("added uniform: %s, location: %i\n", input->name, input->location);
  }
}

static void build_attribute_list(const GLuint programID)
{
  SHADER_PROGRAM_T *program = &shaderPrograms[programID];
  GLint old_attribute_count = program->attribute_array.count;
  
  glGetProgramiv(program->glProgramObj, GL_ACTIVE_ATTRIBUTES, &program->attribute_array.count);
  printf("attribute count: %i\n", program->attribute_array.count);

  glGetProgramiv(program->glProgramObj, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &program->attribute_array.name_max_length);
  
  if (program->attribute_array.count > old_attribute_count) {
    program->attribute_array.start = shader_inputs_count;
    shader_inputs_count += program->attribute_array.count;
  }
  
  const GLuint end = program->attribute_array.count;
	for (GLuint i = 0; i < end; ++i) {
    GPUShaderInput *input = &shader_inputs[program->attribute_array.start + i];
		glGetActiveAttrib(program->glProgramObj, i, 16, NULL, &input->size, &input->type, input->name);

		input->location = glGetAttribLocation(program->glProgramObj, input->name);
    printf("added attribute: %s, location: %i\n", input->name, input->location);
  }
}

void enable_shader_program(PROGRAM_ID_T programID)
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
    if (strcmp(shader_inputs[i].name, name) == 0) {
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
  return get_shader_input_location(&shaderPrograms[active_programID].uniform_array, name);
}

GLint get_active_attribute_location(const char *name)
{
  return get_shader_input_location(&shaderPrograms[active_programID].attribute_array, name);
}

void load_shader_programs(void)
{
  delete_shader_programs();
  load_shaders();
  enable_shader_program(BLINN_PHONG_PRG);
}
