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
static short next_deleted_shader_unit;

static inline short find_deleted_shader_unit_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_shader_unit, shader_units,
                            SHADER_UNIT_MAX_COUNT, GPUShaderUnit, "shader unit");
}

static GPUShaderUnit *get_shader_unit(short id)
{
  if ((id < 0) | (id >= SHADER_UNIT_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Task id, using default id: 0\n");
  }
    
  return shader_units + id;
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

static void shader_unit_init(GPUShaderUnit *shader)
{
  if (shader->glShaderObj) {
    glDeleteShader(shader->glShaderObj);
    shader->glShaderObj = 0;
  }
  shader->modid++;
}

static int find_shader_unit_id(const char *file_name, const GLuint type)
{
  int id = 0;
  for (; id < SHADER_UNIT_MAX_COUNT; id++) {
    if ((shader_units[id].type == type) 
        && (strcmp(shader_units[id].fileName, file_name) == 0)) {
      return id;
    }
  }
  
  return 0;
}

short GPU_shader_unit(const char *file_name, const GLuint type)
{
  short id = find_shader_unit_id(file_name, type);
  GPUShaderUnit *shader;
  
  if (!id) {
    id = find_deleted_shader_unit_id();
    shader = get_shader_unit(id);
    shader->fileName = file_name;
    shader->type = type;

    shader_unit_init(shader);
  }
  else {
    shader = get_shader_unit(id);
  }

  shader->active = 1;

  return id;
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

GLuint GPU_shader_unit_globj(const short id)
{
  GPUShaderUnit *const shader = get_shader_unit(id);
  
  if (!shader->glShaderObj)
    shader_unit_build(shader);
    
  return shader->glShaderObj;
}

void GPU_shader_unit_reset(const short id)
{
  shader_unit_init(get_shader_unit(id));
}

int GPU_shader_unit_modid(const short id)
{
  return get_shader_unit(id)->modid;
}

const char *GPU_shader_unit_file_name(const short id)
{
  return get_shader_unit(id)->fileName;
}


