/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>
#include "gles3.h"

#define BUFFSIZE 5000

// temp shader source buffer for gles2
static char shaderBuf[BUFFSIZE];
static char msg[512];

static GLuint v_shader = 0;
static GLuint f_shader = 0;
static GLuint program = 0;

// The location of the shader uniforms
static GLuint UBO_location;
static GLuint MaterialColor_location;
static GLuint DiffuseMap_location;

// load shader from a file
static int load_shaderBuf_file(const char *name)
{
  FILE *fp;
  int result = 0;
  
  fp = fopen(name, "r");
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


static void make_shader(const GLuint shader, const char *src)
{
  GLint shader_type;
  
  //printf("shader source:\n%s\n", src);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  glGetShaderInfoLog(shader, sizeof msg, NULL, msg);

  glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);
  (shader_type == GL_VERTEX_SHADER) ? printf("vertex") : printf("fragment");
  printf(" shader Compile info: %s\n", msg);

}

static void init_shader_objects(void)
{
   /* Create and attach shaders to program */
  v_shader = glCreateShader(GL_VERTEX_SHADER);
  printf("vertex shader object: %u\n", v_shader);
  f_shader = glCreateShader(GL_FRAGMENT_SHADER);
  printf("fragment shader object: %u\n", f_shader);
  program = glCreateProgram();
  printf("program object: %u\n", program);
  glAttachShader(program, v_shader);
  glAttachShader(program, f_shader);
}

static void bind_attrib_locations(void)
{
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "normal");
  glBindAttribLocation(program, 2, "uv");
}

static void load_shaders(void)
{
  printf("loading shaders...\n");
  /* Compile the vertex shader */
  load_shaderBuf_file("blinn_phong_vert.glsl");
  make_shader(v_shader, shaderBuf);
   
  /* Compile the fragment shader */
  load_shaderBuf_file("blinn_phong_frag.glsl");
  make_shader(f_shader, shaderBuf);
}

void init_shader_system(void)
{
  init_shader_objects();
  bind_attrib_locations();
}

void delete_shader_system(void)
{
  glDeleteShader(v_shader);
  glDeleteShader(f_shader);
  glDeleteProgram(program);	
}

void enable_shader_program()
{

   glLinkProgram(program);
   glGetProgramInfoLog(program, sizeof msg, NULL, msg);
   printf("Link info: %s\n", msg);

   /* Enable the shaders */
   glUseProgram(program);

}

void update_uniform_locations(void)
{
   UBO_location = glGetUniformLocation(program, "UBO");
   printf("UBO_location: %i\n", UBO_location);
   MaterialColor_location = glGetUniformLocation(program, "MaterialColor");
   printf("MaterialColor_location: %i\n", MaterialColor_location);
   DiffuseMap_location = glGetUniformLocation(program, "DiffuseMap");
   printf("DiffuseMap_location: %i\n", DiffuseMap_location);
}

GLuint shader_UBO_location(void)
{
  return UBO_location;
}

GLuint shader_MaterialColor_location(void)
{
  return MaterialColor_location;
}

GLuint shader_DiffuseMap_location(void)
{
  return DiffuseMap_location;
}

void load_shader_program(void)
{
  delete_shader_system();
  init_shader_system();
  load_shaders();
  enable_shader_program();
  update_uniform_locations();
}
