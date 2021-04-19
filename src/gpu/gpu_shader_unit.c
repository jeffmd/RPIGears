// gpu_shader_unit.c

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "id_plug.h"
#include "static_array.h"

#define SHADER_UNIT_MAX_COUNT 10
#define BUFFSIZE 5000

// temp shader source buffer for gles2
static char shaderBuf[BUFFSIZE];
char msg[512];

typedef struct {
  uint8_t active;
  uint8_t modid;
  uint16_t glShaderObj;
  const char *fileName; // if null then represents deleted object
  GLuint type;
} GPUShaderUnit;

static GPUShaderUnit shader_units[SHADER_UNIT_MAX_COUNT];
static ID_t next_deleted_shader_unit;

static inline ID_t find_deleted_shader_unit_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_shader_unit, shader_units,
                            SHADER_UNIT_MAX_COUNT, GPUShaderUnit, "shader unit");
}

static GPUShaderUnit *get_shader_unit(ID_t id)
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

static ID_t find_shader_unit_id(const char *file_name, const GLuint type)
{
  ID_t id = 0;
  for (; id < SHADER_UNIT_MAX_COUNT; id++) {
    if ((shader_units[id].type == type) 
        && (strcmp(shader_units[id].fileName, file_name) == 0)) {
      return id;
    }
  }
  
  return 0;
}

ID_t GPU_shader_unit(const char *file_name, const GLuint type)
{
  ID_t id = find_shader_unit_id(file_name, type);
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

void GPU_shader_unit_reset(const ID_t id)
{
  shader_unit_init(get_shader_unit(id));
}

uint8_t GPU_shader_unit_modid(const ID_t id)
{
  return get_shader_unit(id)->modid;
}

const char *GPU_shader_unit_file_name(const ID_t id)
{
  return get_shader_unit(id)->fileName;
}


