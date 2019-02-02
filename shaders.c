/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>

#include "gpu_shader.h"
#include "gpu_shader_interface.h"

static const char blinn_phong_vert[] = "blinn_phong_vert.glsl";
static const char blinn_phong_frag[] = "blinn_phong_frag.glsl";
static GLuint blinn_phong_prg = 0;


static void load_shaders(void)
{
  printf("loading shaders...\n");
  init_shader_program(blinn_phong_prg);
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

