/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>
#include "gles3.h"

#define BUFFSIZE 5000

// temp shader source buffer for gles2
static char shaderBuf[BUFFSIZE];
static char msg[512];

static GLuint v_shader, f_shader, program;

// load shader from a file
static int loadShaderBufFile(const char *name)
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


static void makeShader(const GLuint shader, const char *src)
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

static void initShaderObjects(void)
{
   /* Create and attach shaders to program */
  v_shader = glCreateShader(GL_VERTEX_SHADER);
  f_shader = glCreateShader(GL_FRAGMENT_SHADER);
  program = glCreateProgram();
  glAttachShader(program, v_shader);
  glAttachShader(program, f_shader);
}

static void bindAttribLocations(void)
{
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "normal");
  glBindAttribLocation(program, 2, "uv");
}

static void loadShaders(void)
{
  /* Compile the vertex shader */
  loadShaderBufFile("blinn_phong_vert.glsl");
  makeShader(v_shader, shaderBuf);
   
  /* Compile the fragment shader */
  loadShaderBufFile("blinn_phong_frag.glsl");
  makeShader(f_shader, shaderBuf);
}

GLuint getShaderProgram(void)
{
  return program;
}

void initShaderSystem(void)
{
  initShaderObjects();
  bindAttribLocations();
  loadShaders();

}

void enableShaderProgram()
{

   glLinkProgram(program);
   glGetProgramInfoLog(program, sizeof msg, NULL, msg);
   printf("Link info: %s\n", msg);

   /* Enable the shaders */
   glUseProgram(program);

}
