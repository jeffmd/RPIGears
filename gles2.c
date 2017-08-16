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
static void draw_gearGLES2(int gearid, GLfloat *transform,
      GLfloat x, GLfloat y, GLfloat angle)
{
   // The direction of the directional light for the scene */
   static const GLfloat LightSourcePosition[4] = { 5.0, 5.0, 10.0, 1.0};

   GLfloat model_view[16];
   GLfloat normal_matrix[16];
   GLfloat model_view_projection[16];

   /* Translate and rotate the gear */
   m4x4_copy(model_view, transform);
   m4x4_translate(model_view, x, y, 0);
   m4x4_rotate(model_view, angle, 0, 0, 1);

   /* Create and set the ModelViewProjectionMatrix */
   m4x4_copy(model_view_projection, state->ProjectionMatrix);
   m4x4_multiply(model_view_projection, model_view);

   glUniformMatrix4fv(state->ModelViewProjectionMatrix_location, 1, GL_FALSE,
                      model_view_projection);
   glUniformMatrix4fv(state->ModelViewMatrix_location, 1, GL_FALSE,
                      model_view);
   /* Set the LightSourcePosition uniform in relation to the object */
   glUniform4fv(state->LightSourcePosition_location, 1, LightSourcePosition);

   glUniform1i(state->DiffuseMap_location, 0);

   /* 
    * Create and set the NormalMatrix. It's the inverse transpose of the
    * ModelView matrix.
    */
   m4x4_copy(normal_matrix, model_view);
   m4x4_invert(normal_matrix);
   m4x4_transpose(normal_matrix);
   glUniformMatrix4fv(state->NormalMatrix_location, 1, GL_FALSE, normal_matrix);

   // Bind texture surface to current vertices
   glBindTexture(GL_TEXTURE_2D, state->texId);

   gear_drawGLES2(gearid, options_useVBO(), options_drawMode(), state->MaterialColor_location);
   
}

/** 
 * Draws the gears in GLES 2 mode.
 */
static void draw_sceneGLES2(void)
{
   GLfloat transform[16];
   m4x4_identity(transform);

   /* Translate and rotate the view */
   m4x4_translate(transform, state->viewX, state->viewY, state->viewDist);
   m4x4_rotate(transform, view_rotx, 1, 0, 0);
   m4x4_rotate(transform, view_roty, 0, 1, 0);
   m4x4_rotate(transform, view_rotz, 0, 0, 1);

   /* Draw the gears */
   draw_gearGLES2(state->gear1, transform, -3.0, -2.0, state->angle);
   draw_gearGLES2(state->gear2, transform, 3.1, -2.0, -2 * state->angle - 9.0);
   draw_gearGLES2(state->gear3, transform, -3.1, 4.2, -2 * state->angle - 25.0);
}

static void init_scene_GLES2(void)
{
   GLuint v, f, program;
   const char *p;
   char msg[512];


   /* Compile the vertex shader */
   p = vertex_shader;
   v = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(v, 1, &p, NULL);
   glCompileShader(v);
   glGetShaderInfoLog(v, sizeof msg, NULL, msg);
   printf("vertex shader info: %s\n", msg);

   /* Compile the fragment shader */
   p = fragment_shader;
   f = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(f, 1, &p, NULL);
   glCompileShader(f);
   glGetShaderInfoLog(f, sizeof msg, NULL, msg);
   printf("fragment shader info: %s\n", msg);

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
   state->ModelViewProjectionMatrix_location = glGetUniformLocation(program, "ModelViewProjectionMatrix");
   state->ModelViewMatrix_location = glGetUniformLocation(program, "ModelViewMatrix");
   state->NormalMatrix_location = glGetUniformLocation(program, "NormalMatrix");
   state->LightSourcePosition_location = glGetUniformLocation(program, "LightSourcePosition");
   state->MaterialColor_location = glGetUniformLocation(program, "MaterialColor");
   state->DiffuseMap_location = glGetUniformLocation(program, "DiffuseMap");

}

static void init_model_projGLES2(void)
{
   /* Update the projection matrix */
   m4x4_perspective(state->ProjectionMatrix, 45.0, (float)window_screen_width() / (float)window_screen_height(), 1.0, 100.0);
	
}

