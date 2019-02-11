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
#include "gpu_shader_interface.h"

static struct {
   GLfloat model_view[16];
   GLfloat LightSourcePosition[4];
   GLfloat projection_matrix[16];
} UBO_Data;

static GLuint DiffuseMap_loc;
static GLuint UBO_loc;
static GLuint MaterialColor_loc;

/**
 * Draws a gear in GLES 2 mode.
 *
 * @param gearid the gear to draw
 * @param x the x position to draw the gear at
 * @param y the y position to draw the gear at
 * @param angle the rotation angle of the gear
 */
static void draw_gear(const int gearid, GLfloat x, GLfloat y, GLfloat angle)
{
   /* Translate and rotate the gear */
   m4x4_copy(UBO_Data.model_view, camera_view_matrix());
   m4x4_translate(UBO_Data.model_view, x, y, 0);
   m4x4_rotate(UBO_Data.model_view, angle, 0, 0, 1);

   glUniform4fv(UBO_loc, 9, (GLfloat *)&UBO_Data);

   gear_draw(gearid, options_drawMode(), MaterialColor_loc);
}

/**
 * Draws the gears in the scene.
 */
void draw_scene(void)
{
  if (light_isDirty() || camera_isDirty()) {
     m4xv3(UBO_Data.LightSourcePosition, camera_view_matrix(), state_LightSourcePosition());
     printf("Recalc Light Position\n");
     light_clean();
  }
   glUniform1i(DiffuseMap_loc, 0);
   // Bind texture surface to current vertices
   GPU_texture_bind(state_texId(), 0);
  /* Draw the gears */
  draw_gear(state_gear1(), -3.0, -2.0, state_angle());
  draw_gear(state_gear2(), 3.1, -2.0, -2 * state_angle() - 9.0);
  draw_gear(state_gear3(), -3.1, 4.2, -2 * state_angle() - 25.0);
}


void init_scene(void)
{
   // setup the scene based on rendering mode
   init_ProjectionMatrix((float)window_screen_width() / (float)window_screen_height());
   //init_shader_system();
   load_shader_programs();
   m4x4_copy(UBO_Data.projection_matrix, camera_ProjectionMatrixPtr());

   DiffuseMap_loc = get_active_uniform_location("DiffuseMap");
   UBO_loc = get_active_uniform_location("UBO")   ;
   MaterialColor_loc = get_active_uniform_location("MaterialColor");
   
   gear_setVAO(state_gear1());
   gear_setVAO(state_gear2());
   gear_setVAO(state_gear3());
}
