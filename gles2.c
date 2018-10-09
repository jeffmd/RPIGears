/*
* gles2.c
*/

static struct {
   GLfloat model_view[16];
   GLfloat LightSourcePosition[4];
   GLfloat projection_matrix[16];
} Data;

/**
 * Draws a gear in GLES 2 mode.
 *
 * @param gear the gear to draw
 * @param transform the current transformation matrix
 * @param x the x position to draw the gear at
 * @param y the y position to draw the gear at
 * @param angle the rotation angle of the gear
 * @param color the color of the gear
 */
static void draw_gearGLES2(const int gearid, GLfloat x, GLfloat y, GLfloat angle)
{
   /* Translate and rotate the gear */
   m4x4_copy(Data.model_view, camera_view_matrix());
   m4x4_translate(Data.model_view, x, y, 0);
   m4x4_rotate(Data.model_view, angle, 0, 0, 1);

   glUniform1i(shader_DiffuseMap_location(), 0);
   
   glUniform4fv(shader_UBO_location(), 9, (GLfloat *)&Data);

   // Bind texture surface to current vertices
   glBindTexture(GL_TEXTURE_2D, state_texId());

   gear_drawGLES2(gearid, options_useVBO(), options_drawMode(), shader_MaterialColor_location());

}

/**
 * Draws the gears in GLES 2 mode.
 */
static void draw_sceneGLES2(void)
{
  if (light_isDirty() || camera_isDirty()) {
     m4xv3(Data.LightSourcePosition, camera_view_matrix(), state_LightSourcePosition());
     printf("Recalc Light Position\n");
     light_clean();
  }
  /* Draw the gears */
  draw_gearGLES2(state_gear1(), -3.0, -2.0, state_angle());
  draw_gearGLES2(state_gear2(), 3.1, -2.0, -2 * state_angle() - 9.0);
  draw_gearGLES2(state_gear3(), -3.1, 4.2, -2 * state_angle() - 25.0);
}


static void init_scene_GLES2(void)
{
   init_shader_system();
   reload_shaders();
   m4x4_copy(Data.projection_matrix, camera_ProjectionMatrixPtr());


   gear_setVAO_GLES2(state_gear1(), options_useVBO());
   gear_setVAO_GLES2(state_gear2(), options_useVBO());
   gear_setVAO_GLES2(state_gear3(), options_useVBO());
}

