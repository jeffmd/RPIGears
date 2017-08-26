/*
* gles2.c
*/

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
static void draw_gearGLES2(const int gearid, GLfloat *view_transform,
      GLfloat x, GLfloat y, GLfloat angle)
{
   // The direction of the directional light for the scene */
   static const GLfloat LightSourcePosition[4] = { 5.0, 5.0, 10.0, 1.0};

   GLfloat model_view[16];
   GLfloat normal_matrix[16];
   GLfloat model_view_projection[16];

   /* Translate and rotate the gear */
   m4x4_copy(model_view, view_transform);
   m4x4_translate(model_view, x, y, 0);
   m4x4_rotate(model_view, angle, 0, 0, 1);

   /* Create and set the ModelViewProjectionMatrix */
   camera_ProjectionMatrix(model_view_projection);
   m4x4_multiply(model_view_projection, model_view);

   glUniformMatrix4fv(state_ModelViewProjectionMatrix_location(), 1, GL_FALSE,
                      model_view_projection);
   glUniformMatrix4fv(state_ModelViewMatrix_location(), 1, GL_FALSE,
                      model_view);
   /* Set the LightSourcePosition uniform in relation to the object */
   glUniform4fv(state_LightSourcePosition_location(), 1, LightSourcePosition);

   glUniform1i(state_DiffuseMap_location(), 0);

   /*
    * Create and set the NormalMatrix. It's the inverse transpose of the
    * ModelView matrix.
    */
   m4x4_copy(normal_matrix, model_view);
   m4x4_invert(normal_matrix);
   m4x4_transpose(normal_matrix);
   glUniformMatrix4fv(state_NormalMatrix_location(), 1, GL_FALSE, normal_matrix);

   // Bind texture surface to current vertices
   glBindTexture(GL_TEXTURE_2D, state_texId());

   gear_drawGLES2(gearid, options_useVBO(), options_drawMode(), state_MaterialColor_location());

}

/**
 * Draws the gears in GLES 2 mode.
 */
static void draw_sceneGLES2(void)
{
  GLfloat view_transform[16];

  camera_view_matrix(view_transform);

  /* Draw the gears */
  draw_gearGLES2(state_gear1(), view_transform, -3.0, -2.0, state_angle());
  draw_gearGLES2(state_gear2(), view_transform, 3.1, -2.0, -2 * state_angle() - 9.0);
  draw_gearGLES2(state_gear3(), view_transform, -3.1, 4.2, -2 * state_angle() - 25.0);
}

static GLuint make_shader(const char *src, const GLenum shader_type)
{
  GLuint shader;
  char msg[512];

  shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  glGetShaderInfoLog(shader, sizeof msg, NULL, msg);
  printf("shader info: %s\n", msg);

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

   glLinkProgram(program);
   glGetProgramInfoLog(program, sizeof msg, NULL, msg);
   printf("info: %s\n", msg);

   /* Enable the shaders */
   glUseProgram(program);

   /* Get the locations of the uniforms so we can access them */
   update_uniform_location(program);

}

