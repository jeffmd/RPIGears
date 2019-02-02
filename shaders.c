/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "gpu_shader_interface.h"

#define SHADER_MAX_COUNT 20
#define SHADER_PROGRAM_MAX_COUNT 10
#define BUFFSIZE 5000

// temp shader source buffer for gles2
static char shaderBuf[BUFFSIZE];
char msg[512];

typedef struct {
  const char *fileName; // if null then represents deleted object
  GLuint type;
  uint16_t glShaderObj;
} SHADER_T;

typedef struct {
  uint16_t shader_vertID;
  uint16_t shader_fragID;
} SHADER_PROGRAM_T;

static SHADER_T shaders[SHADER_MAX_COUNT];
static uint16_t next_deleted_shader = 0;

static SHADER_PROGRAM_T shaderPrograms[SHADER_PROGRAM_MAX_COUNT];
static uint16_t next_deleted_shader_program = 0;

static const char blinn_phong_vert[] = "blinn_phong_vert.glsl";
static const char blinn_phong_frag[] = "blinn_phong_frag.glsl";
static GLuint blinn_phong_prg = 0;

static GLuint find_deleted_shader(void)
{
  GLuint id = next_deleted_shader;
  
  if ((id == 0) | (id >= SHADER_MAX_COUNT)) {
    id = 1;
  }
  
  for (; id < SHADER_MAX_COUNT; id++) {
    if (shaders[id].fileName == 0) {
      next_deleted_shader = id + 1;
      break;
    }
  }
  
  if (id == SHADER_MAX_COUNT) {
    printf("WARNING: No shaders available\n");
    id = 0;
  }
  
	return id;
}

static GLuint find_deleted_shader_program(void)
{
  GLuint id = next_deleted_shader_program;
  
  if ((id == 0) | (id >= SHADER_PROGRAM_MAX_COUNT)) {
    id = 1;
  }
  
  for (; id < SHADER_PROGRAM_MAX_COUNT; id++) {
    if (shaderPrograms[id].shader_vertID == 0) {
      next_deleted_shader_program = id + 1;
      break;
    }
  }
  
  if (id == SHADER_PROGRAM_MAX_COUNT) {
    printf("WARNING: No shader programs available\n");
    id = 0;
  }
  
	return id;
}


static inline GLuint get_shader_obj(const GLuint shaderID)
{
  return shaders[shaderID].glShaderObj;
}

static inline GLuint get_shader_program_vert_obj(const GLuint programID)
{
  return get_shader_obj(shaderPrograms[programID].shader_vertID);
}

static inline GLuint get_shader_program_frag_obj(const GLuint programID)
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

static GLuint shader_create(const char *file_name, const GLuint type)
{
  const GLuint shaderID = find_deleted_shader();
  SHADER_T *shader = &shaders[shaderID];
  shader->fileName = file_name;
  shader->type = type;
    
  return shaderID;
}

static void init_shader(const GLuint shaderID)
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

static void init_shader_program(const GLuint programID)
{
  SHADER_PROGRAM_T *program = &shaderPrograms[programID];
  
  init_shader(program->shader_vertID);
  init_shader(program->shader_fragID);
  
  GLuint gl_program = init_shader_interface(programID);
  
  glAttachShader(gl_program, get_shader_obj(program->shader_vertID));
  glAttachShader(gl_program, get_shader_obj(program->shader_fragID));
}

GLuint shader_program_create(const char *vertex_file_name, const char *fragment_file_name)
{
  const GLuint programID = find_deleted_shader_program();
  
  SHADER_PROGRAM_T *program = &shaderPrograms[programID];
  
  program->shader_vertID = shader_create(vertex_file_name, GL_VERTEX_SHADER);
  program->shader_fragID = shader_create(fragment_file_name, GL_FRAGMENT_SHADER);
  
  return programID;
}

static void load_shaders(void)
{
  printf("loading shaders...\n");
  init_shader_program(blinn_phong_prg);
}

static void reset_shader_program(const GLuint programID)
{
  glUseProgram(0);

  const SHADER_PROGRAM_T *program = &shaderPrograms[programID];

  glDeleteShader(get_shader_obj(program->shader_vertID));
  glDeleteShader(get_shader_obj(program->shader_fragID));
  glDeleteProgram(get_shader_program_obj(programID));
}

void delete_shader_programs(void)
{
  reset_shader_program(blinn_phong_prg);
}

void load_shader_programs(void)
{
  if (!blinn_phong_prg)
    blinn_phong_prg = shader_program_create(blinn_phong_vert, blinn_phong_frag);
    
  delete_shader_programs();
  load_shaders();
  enable_shader_program(blinn_phong_prg);
}

