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

#define MAX_TEXTURES 5
typedef struct {
  short quad;
  short textures[MAX_TEXTURES];
  float alimits[MAX_TEXTURES];
  uint8_t texture_count;
  uint8_t texture_index;
  uint8_t toggle;
} TQuad;

static TQuad quad;

static void test_quad_toggle(const short souce_id, const short destination_id)
{
  quad.toggle = !quad.toggle;
}

static void test_quad_next_texture(const short souce_id, const short destination_id)
{
  quad.texture_index++;
  if (quad.texture_index >= quad.texture_count)
    quad.texture_index = 0;
  GPU_quad_set_texture(quad.quad, quad.textures[quad.texture_index]);
  GPU_quad_set_alimit(quad.quad, quad.alimits[quad.texture_index]);
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

  quad.texture_count = 0;
  GPU_quad_set_shader(quad.quad, Shaders_test_quad());
  Key_add_action(SHIFT_KEY('T'), test_quad_toggle, "toggle test quad visibility");
  Key_add_action(SHIFT_KEY('Y'), test_quad_next_texture, "next test quad texture");
  atexit(test_quad_delete);
  
}

void test_quad_draw(void)
{
  if (quad.toggle) {
    GPU_quad_draw(quad.quad);  
  }
}

void test_quad_add_texture(const int id, const float alimit)
{
  GPU_quad_set_texture(quad.quad, id);
  quad.texture_index = quad.texture_count;
  quad.textures[quad.texture_index] = id;
  quad.alimits[quad.texture_index] = alimit;

  quad.texture_count++;
  if (quad.texture_count >= MAX_TEXTURES)
    quad.texture_count = MAX_TEXTURES - 1;
}
