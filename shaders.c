/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>

#include "gles3.h"
#include "gpu_shader.h"

static const char blinn_phong_vert[] = "blinn_phong_vert.glsl";
static const char blinn_phong_frag[] = "blinn_phong_frag.glsl";
static GPUShader *blinn_phong_prg = 0;


void load_shader_programs(void)
{
  if (!blinn_phong_prg)
    blinn_phong_prg = GPU_shader_create(blinn_phong_vert, blinn_phong_frag);
    
  GPU_shader_reset(blinn_phong_prg);
  GPU_shader_bind(blinn_phong_prg);
}

