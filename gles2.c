/*
* gles2.c
*/

static struct {
   GLfloat model_view[16];
   GLfloat LightSourcePosition[4];
   GLfloat normal_matrix[16];
   GLfloat projection_matrix[16];
} Data = { .LightSourcePosition = { 0.0, 0.0, 30.0, 1.0}};

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

   glUniform1i(state_DiffuseMap_location(), 0);
   
   glUniform4fv(state_UBO_location(), 13, (GLfloat *)&Data);

   // Bind texture surface to current vertices
   glBindTexture(GL_TEXTURE_2D, state_texId());

   gear_drawGLES2(gearid, options_useVBO(), options_drawMode(), state_MaterialColor_location());

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

static GLuint make_shader(const char *src, const GLenum shader_type)
{
  GLuint shader;
  char msg[512];

  shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  glGetShaderInfoLog(shader, sizeof msg, NULL, msg);
  printf("shader Compile info: %s\n", msg);

  return shader;
}

static void init_scene_GLES2(void)
{
   GLuint v, f, program;
  char msg[512];

   /* Compile the vertex shader */
   v = make_shader(vertex_shader, GL_VERTEX_SHADER);

   /* Compile the fragment shader */
   f = make_shader(fragment_shader, GL_FRAGMENT_SHADER);

   /* Create and link the shader program */
   program = glCreateProgram();
   glAttachShader(program, v);
   glAttachShader(program, f);
   glBindAttribLocation(program, 0, "position");
   glBindAttribLocation(program, 1, "normal");
   glBindAttribLocation(program, 2, "uv");

   m4x4_copy(Data.projection_matrix, camera_ProjectionMatrixPtr());

   glLinkProgram(program);
   glGetProgramInfoLog(program, sizeof msg, NULL, msg);
   printf("Link info: %s\n", msg);

   /* Enable the shaders */
   glUseProgram(program);

   /* Get the locations of the uniforms so we can access them */
   update_uniform_location(program);
   gear_setVAO_GLES2(state_gear1(), options_useVBO());
   gear_setVAO_GLES2(state_gear2(), options_useVBO());
   gear_setVAO_GLES2(state_gear3(), options_useVBO());
}

