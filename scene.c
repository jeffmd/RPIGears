/*
* scene.c
*/

static struct {
   GLfloat model_view[16];
   GLfloat LightSourcePosition[4];
   GLfloat projection_matrix[16];
} Data;

static GLuint DiffuseMap_loc;
static GLuint UBO_loc;
static GLuint MaterialColor_loc;

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
static void draw_gear(const int gearid, GLfloat x, GLfloat y, GLfloat angle)
{
   /* Translate and rotate the gear */
   m4x4_copy(Data.model_view, camera_view_matrix());
   m4x4_translate(Data.model_view, x, y, 0);
   m4x4_rotate(Data.model_view, angle, 0, 0, 1);

   glUniform1i(DiffuseMap_loc, 0);
   // Bind texture surface to current vertices
   GPU_texture_bind(state_texId(), 0);
   
   glUniform4fv(UBO_loc, 9, (GLfloat *)&Data);


   gear_draw(gearid, options_drawMode(), MaterialColor_loc);

}

/**
 * Draws the gears in GLES 2 mode.
 */
static void draw_scene(void)
{
  if (light_isDirty() || camera_isDirty()) {
     m4xv3(Data.LightSourcePosition, camera_view_matrix(), state_LightSourcePosition());
     printf("Recalc Light Position\n");
     light_clean();
  }
  /* Draw the gears */
  draw_gear(state_gear1(), -3.0, -2.0, state_angle());
  draw_gear(state_gear2(), 3.1, -2.0, -2 * state_angle() - 9.0);
  draw_gear(state_gear3(), -3.1, 4.2, -2 * state_angle() - 25.0);
}


static void init_scene(void)
{
   // setup the scene based on rendering mode
   init_ProjectionMatrix((float)window_screen_width() / (float)window_screen_height());
   //init_shader_system();
   load_shader_programs();
   m4x4_copy(Data.projection_matrix, camera_ProjectionMatrixPtr());

   DiffuseMap_loc = get_active_uniform_location("DiffuseMap");
   UBO_loc = get_active_uniform_location("UBO")   ;
   MaterialColor_loc = get_active_uniform_location("MaterialColor");
   
   gear_setVAO(state_gear1());
   gear_setVAO(state_gear2());
   gear_setVAO(state_gear3());
}
