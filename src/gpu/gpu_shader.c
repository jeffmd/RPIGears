// gpu_shader.c

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "static_array.h"
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
  uint8_t active;
  GLint linked;
  int modid;
  short vert_unit;
  int vert_modid;
  short frag_unit;
  int frag_modid;
  uint16_t glProgramObj;
  ShaderInputArrayTracker uniforms;
  ShaderInputArrayTracker attributes;
} GPUShader;

static GPUShader shaders[SHADER_MAX_COUNT];
static short next_deleted_shader;


static GPUShaderInput shader_inputs[SHADER_INPUTS_MAX_COUNT];
static uint16_t shader_inputs_count = 0;

static short active_shader = 0;
static int active_shader_modid = 0;

static inline int find_deleted_shader_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_shader, shaders,
                            SHADER_MAX_COUNT, GPUShader, "shader");
}

static GPUShader *get_shader(int id)
{
  if ((id < 0) | (id >= SHADER_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad shader id, using default id: 0\n");
  }
    
  return shaders + id;
}

static void shader_init(GPUShader *shader)
{
  if (shader->glProgramObj) {
    glDeleteProgram(shader->glProgramObj);
    shader->glProgramObj = 0;
  }

  shader->modid++;
  shader->linked = 0;
}

static void shader_attach(GPUShader *shader)
{
  shader->glProgramObj = glCreateProgram();
  printf("program object: %u\n", shader->glProgramObj);
  shader->linked = 0;
  
  glAttachShader(shader->glProgramObj, GPU_shader_unit_globj(shader->vert_unit));
  shader->vert_modid = GPU_shader_unit_modid(shader->vert_unit);
  glAttachShader(shader->glProgramObj, GPU_shader_unit_globj(shader->frag_unit));
    shader->frag_modid = GPU_shader_unit_modid(shader->frag_unit);

  shader->modid++;
}

static int find_shader(const char *vertex_file_name, const char *fragment_file_name)
{
  int id = 1;
  for (; id < SHADER_MAX_COUNT; id++) {
    GPUShader *shader = shaders + id;
    
    if (shader->vert_unit && shader->frag_unit) { 
        if ((strcmp(GPU_shader_unit_file_name(shader->vert_unit), vertex_file_name) == 0)
             && (strcmp(GPU_shader_unit_file_name(shader->frag_unit), fragment_file_name) == 0) ) {
          return id;
        }
    }
  }
  
  return 0;
}

int GPU_shader(const char *vertex_file_name, const char *fragment_file_name)
{
  int id = find_shader(vertex_file_name, fragment_file_name);
  GPUShader *shader;
  
  if (!id) {
    id = find_deleted_shader_id();
    shader = get_shader(id);
    shader_init(shader);
    
    shader->vert_unit = GPU_shader_unit(vertex_file_name, GL_VERTEX_SHADER);
    shader->vert_modid = GPU_shader_unit_modid(shader->vert_unit);
    shader->frag_unit = GPU_shader_unit(fragment_file_name, GL_FRAGMENT_SHADER);
    shader->frag_modid = GPU_shader_unit_modid(shader->frag_unit);
  }
  else {
    shader = get_shader(id);
  }
  
  shader->active = 1;
    
  return id;
}

void GPU_shader_reset(const int id)
{
  if (active_shader == id)
    glUseProgram(0);

  GPUShader *const shader = get_shader(id);

  GPU_shader_unit_reset(shader->vert_unit);
  GPU_shader_unit_reset(shader->frag_unit);
  shader_init(shader);
}


static void update_array_tracker(GPUShader *shader, const GLboolean is_uniform)
{
  ShaderInputArrayTracker *tracker = is_uniform ? &shader->uniforms : &shader->attributes;
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
  ShaderInputArrayTracker *tracker = is_uniform ? &shader->uniforms : &shader->attributes;


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

static void build_uniforms(GPUShader *shader)
{
  update_array_tracker(shader, GL_TRUE);
  build_input_list(shader, GL_TRUE);
}

static void build_attributes(GPUShader *shader)
{
  update_array_tracker(shader, GL_FALSE);
  build_input_list(shader, GL_FALSE);
}

static void shader_link(GPUShader *shader)
{
  glLinkProgram(shader->glProgramObj);
  glGetProgramInfoLog(shader->glProgramObj, sizeof msg, NULL, msg);
  printf("Link info: %s\n", msg);
  build_uniforms(shader);
  build_attributes(shader);
  shader->linked = 1;
  shader->modid++;
}

static void shader_check_unit_updates(GPUShader *shader)
{
  if ((shader->vert_modid != GPU_shader_unit_modid(shader->vert_unit))
     || (shader->frag_modid != GPU_shader_unit_modid(shader->frag_unit)) ) {
    shader_init(shader);   
  }
  
}

void GPU_shader_bind(const int id)
{
  GPUShader *const shader = get_shader(id);

  shader_check_unit_updates(shader);
  
  if ((active_shader != id) || (shader->modid != active_shader_modid)) {
    if (!shader->glProgramObj)
      shader_attach(shader);
      
    if (!shader->linked)
      shader_link(shader);

    /* Enable the shaders */
    glUseProgram(shader->glProgramObj);
    active_shader = id;
    active_shader_modid = shader->modid;
  }
}

int GPU_shader_modid(const int id)
{
  return get_shader(id)->modid;
}

int GPU_shader_active_shader(void)
{
  return active_shader;
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

GLint GPU_shader_uniform_location(const int id, const char *name)
{
  return get_shader_input_location(&get_shader(id)->uniforms, name);
}

GLint GPU_shader_attribute_location(const int id, const char *name)
{
  return get_shader_input_location(&get_shader(id)->attributes, name);
}

GLint GPU_get_active_uniform_location(const char *name)
{
  return get_shader_input_location(&get_shader(active_shader)->uniforms, name);
}

GLint GPU_get_active_attribute_location(const char *name)
{
  return get_shader_input_location(&get_shader(active_shader)->attributes, name);
}

