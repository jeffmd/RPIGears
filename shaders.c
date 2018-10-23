/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "shaderid.h"

#define BUFFSIZE 5000

// temp shader source buffer for gles2
static char shaderBuf[BUFFSIZE];
static char msg[512];

typedef struct {
  const char *name;
  GLint location;  
} UNIFORM_T;

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
} SHADER_PROGRAM_T;


SHADER_T shaders[] = {
  [BLINN_PHONG_VS] = {"blinn_phong_vert.glsl", GL_VERTEX_SHADER, 0},
  [BLINN_PHONG_FS] = {"blinn_phong_frag.glsl", GL_FRAGMENT_SHADER, 0}
};

SHADER_PROGRAM_T shaderPrograms[] = {
  [BLINN_PHONG_PRG] = {BLINN_PHONG_VS, BLINN_PHONG_FS, 0}
};

UNIFORM_T uniforms[] = {
  [U_UBO] = {"UBO", -1},
  [U_MATERIALCOLOR] = {"MaterialColor", -1},
  [U_DIFFUSEMAP] = {"DiffuseMap", -1}
};

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

static void bind_attrib_locations(void)
{
  GLuint program = get_shader_program_obj(BLINN_PHONG_PRG);

  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "normal");
  glBindAttribLocation(program, 2, "uv");
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

void enable_shader_program(PROGRAM_ID_T programID)
{
  const GLuint program = get_shader_program_obj(programID); 

  glLinkProgram(program);
  glGetProgramInfoLog(program, sizeof msg, NULL, msg);
  printf("Link info: %s\n", msg);

  /* Enable the shaders */
  glUseProgram(program);

}

static void update_uniform_location(const GLuint program,  UNIFORM_ID_T id)
{
  UNIFORM_T *uniform = &uniforms[id];
  
  uniform->location = glGetUniformLocation(program, uniform->name);
  printf("Uniform: %s location: %i\n", uniform->name, uniform->location);
}

void update_uniform_locations(void)
{
  const GLuint program = get_shader_program_obj(BLINN_PHONG_PRG);
  
  update_uniform_location(program, U_UBO);
  update_uniform_location(program, U_MATERIALCOLOR);
  update_uniform_location(program, U_DIFFUSEMAP);
}

GLint get_uniform_location(const UNIFORM_ID_T id)
{
  return uniforms[id].location;
}

void load_shader_programs(void)
{
  delete_shader_programs();
  load_shaders();
  bind_attrib_locations();
  enable_shader_program(BLINN_PHONG_PRG);
  update_uniform_locations();
}
