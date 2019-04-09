// gpu_shader_unit.c

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "gles3.h"

#define SHADER_UNIT_MAX_COUNT 10
#define BUFFSIZE 5000

// temp shader source buffer for gles2
static char shaderBuf[BUFFSIZE];
char msg[512];

typedef struct {
  const char *fileName; // if null then represents deleted object
  GLuint type;
  uint16_t glShaderObj;
} GPUShaderUnit;

static GPUShaderUnit shader_units[SHADER_UNIT_MAX_COUNT];
static GPUShaderUnit *next_deleted_shader_unit = 0;

static GPUShaderUnit *find_deleted_shader_unit(void)
{
  GPUShaderUnit *shader = next_deleted_shader_unit;
  const GPUShaderUnit *max_shader = shader_units + SHADER_UNIT_MAX_COUNT;
  
  if ((shader <= shader_units) | (shader >= max_shader)) {
    shader = shader_units + 1;
  }
  
  for (; shader < max_shader; shader++) {
    if (shader->fileName == 0) {
      next_deleted_shader_unit = shader + 1;
      break;
    }
  }
  
  if (shader == max_shader) {
    printf("WARNING: No shader units available\n");
    shader = shader_units;
  }
  
  return shader;
}

GLuint GPU_shader_unit_globj(GPUShaderUnit *shader)
{
  return shader->glShaderObj;
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

GPUShaderUnit *GPU_shader_unit_create(const char *file_name, const GLuint type)
{
  GPUShaderUnit *shader = find_deleted_shader_unit();

  shader->fileName = file_name;
  shader->type = type;
    
  return shader;
}

void GPU_shader_unit_init(GPUShaderUnit *shader)
{
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

void GPU_shader_unit_glDelete(GPUShaderUnit *shader)
{
  glDeleteShader(shader->glShaderObj);
  shader->glShaderObj = 0;
}