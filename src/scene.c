/*
* scene.c
*/

#include <stdio.h>

#include "gles3.h"
#include "gear.h"
#include "matrix_math.h"
#include "camera.h"
#include "gpu_texture.h"
#include "demo_state.h"
#include "user_options.h"
#include "window.h"
#include "shaders.h"
#include "gpu_shader.h"
#include "gpu_uniform_buffer.h"
#include "key_input.h"

static struct {
   GLfloat model_view[16];
   GLfloat LightSourcePosition[4];
   GLfloat projection_matrix[16];
} UBO_Data;

static GLfloat tm[16];
static short diffuseMap_Data = 0;
static short uniform_buffer;

/**
 * Draws a gear in GLES 2 mode.
 *
 * @param gear the gear to draw
 * @param x the x position to draw the gear at
 * @param y the y position to draw the gear at
 * @param angle the rotation angle of the gear
 */
static void draw_gear(const int gear, const GLfloat x, const GLfloat y, const GLfloat angle)
{
   /* Translate and rotate the gear */
   m4x4_translate(tm, camera_view_matrix(), x, y, 0);
   m4x4_rotate(UBO_Data.model_view, tm, angle, 0, 0, 1);

   gear_draw(gear, options_drawMode(), state_instances());
}

/**
 * Draws the gears in the scene.
 */
void scene_draw(void)
{
  if (light_isDirty() || camera_isDirty()) {
     m4xv3(UBO_Data.LightSourcePosition, camera_view_matrix(), state_LightSourcePosition());
     printf("Recalc Light Position\n");
     light_clean();
  }
  
  // Bind texture surface to current vertices
  GPU_texture_bind(state_tex(), 0);
  shaders_bind_gear_shader();
  // make uniform_buffer active so that it gets used for all active gears
  GPU_uniformbuffer_activate(uniform_buffer);
  /* Draw the gears */
  draw_gear(state_gear1(), -3.0, -2.0, state_angle());
  draw_gear(state_gear2(), 3.1, -2.0, -2 * state_angle() - 9.0);
  draw_gear(state_gear3(), -3.1, 4.2, -2 * state_angle() - 25.0);
}


void scene_init(void)
{
  // setup the scene based on rendering mode
  camera_init_ProjectionMatrix((float)window_screen_width() / (float)window_screen_height());
  key_add_action('R', shaders_load_programs, "reload shaders");
  m4x4_copy(UBO_Data.projection_matrix, camera_ProjectionMatrixPtr());

  uniform_buffer = GPU_uniformbuffer_create();
  GPU_uniformbuffer_add_4f(uniform_buffer, "UBO", UBO_Data);
  GPU_uniformbuffer_add_1i(uniform_buffer, "DiffuseMap", diffuseMap_Data);
}
