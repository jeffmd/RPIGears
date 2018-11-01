/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "gpu_shader_interface.h"

#define BUFFSIZE 5000

// temp shader source buffer for gles2
static char shaderBuf[BUFFSIZE];
char msg[512];

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
} SHADER_PROGRAM_T;

SHADER_T shaders[] = {
  [BLINN_PHONG_VS] = {"blinn_phong_vert.glsl", GL_VERTEX_SHADER, 0},
  [BLINN_PHONG_FS] = {"blinn_phong_frag.glsl", GL_FRAGMENT_SHADER, 0}
};

static SHADER_PROGRAM_T shaderPrograms[] = {
  [BLINN_PHONG_PRG] = {BLINN_PHONG_VS, BLINN_PHONG_FS}
};

static inline GLuint get_shader_obj(SHADER_ID_T shaderID)
{
  return shaders[shaderID].glShaderObj;
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
  
  GLuint gl_program = init_shader_interface(programID);
  
  glAttachShader(gl_program, get_shader_obj(program->shader_vertID));
  glAttachShader(gl_program, get_shader_obj(program->shader_fragID));
}

static void load_shaders(void)
{
  printf("loading shaders...\n");
  init_shader_program(BLINN_PHONG_PRG);
}

static void delete_shader_program(const PROGRAM_ID_T programID)
{
  glUseProgram(0);

  const SHADER_PROGRAM_T *program = &shaderPrograms[programID];

  glDeleteShader(get_shader_obj(program->shader_vertID));
  glDeleteShader(get_shader_obj(program->shader_fragID));
  glDeleteProgram(get_shader_program_obj(programID));
}

void delete_shader_programs(void)
{
  delete_shader_program(BLINN_PHONG_PRG);
}

void load_shader_programs(void)
{
  delete_shader_programs();
  load_shaders();
  enable_shader_program(BLINN_PHONG_PRG);
}
