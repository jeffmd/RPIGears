/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>

#include "gles3.h"
#include "gpu_shader.h"

static const char blinn_phong_vert[] = "blinn_phong_vert.glsl";
static const char blinn_phong_frag[] = "blinn_phong_frag.glsl";
static const char test_quad_vert[] = "test_quad_vert.glsl";
static const char test_quad_frag[] = "test_quad_frag.glsl";

static int blinn_phong_prg = 0;
static int test_quad_prg = 0;

static int active_gear_shader = 0;
static int active_test_quad_shader = 0;

void shaders_load_programs(void)
{
  if (!blinn_phong_prg) {
    blinn_phong_prg = GPU_shader(blinn_phong_vert, blinn_phong_frag);
  }

  if (!test_quad_prg) {
    test_quad_prg = GPU_shader(test_quad_vert, test_quad_frag);
  }
  
  GPU_shader_reset(blinn_phong_prg);
  active_gear_shader = blinn_phong_prg;
  GPU_shader_reset(test_quad_prg);
  active_test_quad_shader = test_quad_prg;
}

void shaders_bind_gear_shader(void)
{
  GPU_shader_bind(active_gear_shader);
}

void shaders_bind_test_quad_shader(void)
{
  GPU_shader_bind(active_test_quad_shader);
}

