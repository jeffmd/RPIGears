// gpu_shader_unit.c

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "static_array.h"

#define SHADER_UNIT_MAX_COUNT 10
#define BUFFSIZE 5000

// temp shader source buffer for gles2
static char shaderBuf[BUFFSIZE];
char msg[512];

typedef struct {
  uint8_t active;
  const char *fileName; // if null then represents deleted object
  GLuint type;
  uint16_t glShaderObj;
  int modid;
} GPUShaderUnit;

static GPUShaderUnit shader_units[SHADER_UNIT_MAX_COUNT];
static GPUShaderUnit *next_deleted_shader_unit = 0;

static inline GPUShaderUnit *find_deleted_shader_unit(void)
{
  return ARRAY_FIND_DELETED(next_deleted_shader_unit, shader_units,
                            SHADER_UNIT_MAX_COUNT, "shader unit");
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

void shader_unit_init(GPUShaderUnit *shader)
{
  shader->glShaderObj = 0;
  shader->modid++;
}

GPUShaderUnit *find_shader_unit(const char *file_name, const GLuint type)
{
  
  for (int idx = 0; idx < SHADER_UNIT_MAX_COUNT; idx++) {
    if ((shader_units[idx].type == type) 
        && (strcmp(shader_units[idx].fileName, file_name) == 0)) {
      return &shader_units[idx];
    }
  }
  
  return 0;
}

GPUShaderUnit *GPU_shader_unit(const char *file_name, const GLuint type)
{
  GPUShaderUnit *shader = find_shader_unit(file_name, type);
  
  if (!shader) {
    shader = find_deleted_shader_unit();

    shader->fileName = file_name;
    shader->type = type;
    shader_unit_init(shader);
  }  

  return shader;
}

static void shader_unit_build(GPUShaderUnit *shader)
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
  
  shader->modid++;

}

GLuint GPU_shader_unit_globj(GPUShaderUnit *shader)
{
  if (!shader->glShaderObj)
    shader_unit_build(shader);
    
  return shader->glShaderObj;
}

void GPU_shader_unit_reset(GPUShaderUnit *shader)
{
  glDeleteShader(shader->glShaderObj);
  shader_unit_init(shader);
}

int GPU_shader_unit_modid(GPUShaderUnit *shader)
{
  return shader->modid;
}

const char *GPU_shader_unit_file_name(GPUShaderUnit *shader)
{
  return shader->fileName;
}


