/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>

#define BUFFSIZE 5000

// vertex shader for gles2
char vertex_shader[BUFFSIZE];
// fragment shader for gles2
char fragment_shader[BUFFSIZE];

// load shader from a file
static int loadShader(const char *name, char *buffer)
{
  FILE *fp;
  int result = 0;
  
  fp = fopen(name, "r");
  if (fp) {
    result = fread(buffer, sizeof(char), BUFFSIZE, fp);
    buffer[result - 1] = 0;
    fclose(fp);
    printf("shader %s loaded\n", name);
  }
  else {
    printf("failed to load %s\n", name);
  }
  
  return result;
}

void initShaderSource(void)
{
  printf("loading shader sources:\n");
  loadShader("blinn_phong_vert.glsl", vertex_shader);
  loadShader("blinn_phong_frag.glsl", fragment_shader);
}
