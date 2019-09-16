// test_quad.c
// render a texture to a quad

#include <stdlib.h>
#include <stdio.h>

#include "gles3.h"

#include "gpu_quad.h"
#include "gpu_uniform_buffer.h"
#include "shaders.h"
#include "key_input.h"

enum {
  ATTR_POSITION,
};

typedef struct {
  short quad;
  uint8_t toggle;
} TQuad;

static TQuad quad;

static void test_quad_toggle(void)
{
  quad.toggle = !quad.toggle;
}

static void test_quad_delete(void)
{
  GPU_quad_delete(quad.quad);
  quad.quad = 0;
  printf("test quad has shut down\n"); 
}

void test_quad(void)
{
  if (!quad.quad) {
    quad.quad = GPU_quad_create();    
  }

  GPU_quad_set_shader(quad.quad, shaders_test_quad());
  key_add_action('T', test_quad_toggle, "toggle test quad visibility");
  atexit(test_quad_delete);
  
}

void test_quad_draw(void)
{
  if (quad.toggle) {
    GPU_uniformbuffer_activate(0);
    GPU_quad_draw(quad.quad);  
  }
}

void test_quad_set_texture(const int id)
{
  GPU_quad_set_texture(quad.quad, id);
}
