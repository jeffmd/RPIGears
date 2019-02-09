/*
 * print_info.c
 */

#include <stdio.h>

#include "gles3.h"
#include "EGL/egl.h"

void print_GLInfo(void)
{
  printf("\nGL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
  printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
  printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
  printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
  printf("GLSL VERSION  = %s\n", (char *) glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void print_GL_Limits(void)
{
  GLint num[4];

  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, num);
  printf("MAX_VERTEX_ATTRIBS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_VARYING_VECTORS, num);
  printf("MAX_VARYING_VECTORS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, num);
  printf("MAX_FRAGMENT_UNIFORM_VECTORS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, num);
  printf("MAX_VERTEX_UNIFORM_VECTORS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, num);
  printf("MAX_VERTEX_TEXTURE_IMAGE_UNITS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, num);
  printf("MAX_TEXTURE_IMAGE_UNITS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, num);
  printf("MAX_COMBINED_TEXTURE_IMAGE_UNITS: %i\n", num[0]);

  glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, num);
  printf("NUM_COMPRESSED_TEXTURE_FORMATS: %i\n", num[0]);

  glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, num);
  printf("NUM_SHADER_BINARY_FORMATS: %i\n", num[0]);

  glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, num);
  printf("GL_ALIASED_LINE_WIDTH_RANGE: %i, %i\n", num[0], num[1]);

  glGetIntegerv(GL_ALIASED_POINT_SIZE_RANGE, num);
  printf("GL_ALIASED_POINT_SIZE_RANGE: %i, %i\n", num[0], num[1]);

  glGetIntegerv(GL_MAX_VIEWPORT_DIMS, num);
  printf("GL_MAX_VIEWPORT_DIMS: %i, %i\n", num[0], num[1]);
}

void print_EGL_info(void)
{
  const EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  
  printf("EGL version : %s\n", eglQueryString(display, EGL_VERSION));
  printf("EGL Vendor : %s\n", eglQueryString(display, EGL_VENDOR));
  printf("EGL extensions : %s\n", eglQueryString(display, EGL_EXTENSIONS));
}

void print_EGLSurface_info(void *surface)
{
  const EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  int value;
  
  if (eglQuerySurface(display, surface, EGL_SWAP_BEHAVIOR, &value)) {
    printf("EGL_SWAP_BEHAVIOR: ");  
	  if (value == EGL_BUFFER_PRESERVED)
      printf("EGL_BUFFER_PRESERVED\n");
    else
      printf("EGL_BUFFER_DESTROYED\n");
  }
}

void print_keyhelp(void)
{
  printf(
   "\nspecial keys and what they do\n"
   "i - print GL info\n"
   "l - toggle draw mode GL_TRIAGLES/GL_LINES\n"
   "o - print command line options\n"
   "p - turn gears spin off\n"
   "v - toggle vertical sync on/off\n"
   "z - increase window size (zoom in)\n"
   "Z - decrease window size (zoom out)\n"
   "< - decrease gear spin rate\n"
   "> - increase gear spin rate\n"
   "a - move camera left\n"
   "d - move camera right\n"
   "w - move camera up\n"
   "s - move camera down\n"
   "r - move camera back from gears\n"
   "f - move camera toward gears\n"
   "up arrow - move light up\n"
   "down arrow - move light down\n"
   "left arrow - move light left\n"
   "right arrow - move light right\n"
   "home - move window to centre of screen\n"
   "end - move window off screen\n"
   );
}

void print_CLoptions_help(void)
{
  printf(
    "\nusage: RPIGears [options]\n"
    "options: -vsync | -exit | -info | -vbo | -gles2 | -line | -nospin\n"
    "-vsync: wait for vertical sync before new frame is displayed\n"
    "-exit: automatically exit RPIGears after 30 seconds\n"
    "-info: display opengl driver info\n"
    "-vbo: use vertex buffer object in GPU memory\n"
    "-line: draw lines only, wire frame mode\n"
    "-nospin: gears don't turn\n"
    );
}

