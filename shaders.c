/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>

#include "gles3.h"
#include "gpu_shader.h"

static const char blinn_phong_vert[] = "blinn_phong_vert.glsl";
static const char blinn_phong_frag[] = "blinn_phong_frag.glsl";
static GPUShader *blinn_phong_prg = 0;

static GPUShader *active_gear_shader = 0;


void shaders_load_programs(void)
{
  if (!blinn_phong_prg) {
    blinn_phong_prg = GPU_shader(blinn_phong_vert, blinn_phong_frag);
  }
  
  GPU_shader_reset(blinn_phong_prg);
  active_gear_shader = blinn_phong_prg;
}

void shaders_bind_gear_shader(void)
{
  GPU_shader_bind(active_gear_shader);
}

