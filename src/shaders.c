/*
 * shaders.c - vertex and pixel shaders used in gles2 mode
 */

#include <stdio.h>

#include "gles3.h"
#include "static_array.h"
#include "gpu_shader.h"

static const char blinn_phong_vert[] = "blinn_phong_vert.glsl";
static const char blinn_phong_frag[] = "blinn_phong_frag.glsl";
static const char test_quad_vert[] = "test_quad_vert.glsl";
static const char test_quad_frag[] = "test_quad_frag.glsl";
static const char line_art_vert[] = "line_art_vert.glsl";
static const char line_art_frag[] = "line_art_frag.glsl";

static ID_t blinn_phong_shader;
static ID_t test_quad_shader;
static ID_t line_art_shader;

static ID_t active_gear_shader;
static ID_t active_test_quad_shader;

void Shaders_load_programs(void)
{
  if (!blinn_phong_shader) {
    blinn_phong_shader = GPU_shader(blinn_phong_vert, blinn_phong_frag);
  }

  if (!test_quad_shader) {
    test_quad_shader = GPU_shader(test_quad_vert, test_quad_frag);
  }
  
  GPU_shader_reset(blinn_phong_shader);
  active_gear_shader = blinn_phong_shader;
  GPU_shader_reset(test_quad_shader);
  active_test_quad_shader = test_quad_shader;
}

void Shaders_bind_gear_shader(void)
{
  if (!active_gear_shader)
    Shaders_load_programs();

  GPU_shader_bind(active_gear_shader);
}

ID_t Shaders_test_quad(void)
{
  if (!active_test_quad_shader)
    Shaders_load_programs();

  return active_test_quad_shader;
}

void Shaders_bind_line_art(void)
{
  if (!line_art_shader) {
    line_art_shader = GPU_shader(line_art_vert, line_art_frag);
  }

  GPU_shader_bind(line_art_shader);
}

