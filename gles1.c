/*
* gles1.c
*/

void draw_gearGLES1(const int gearid, const GLfloat x, const GLfloat y, const GLfloat angle)
{

  glPushMatrix();
  glTranslatef(x, y, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);
	
  // Bind texture surface to current vertices
  glBindTexture(GL_TEXTURE_2D, state_texId());

  gear_drawGLES1(gearid, options_useVBO(), options_drawMode());
                   
  glPopMatrix();
  
}

static void draw_sceneGLES1(void)
{
  glPushMatrix();

  glTranslatef(state_viewX(), state_viewY(), state_viewDist());

  glRotatef(view_rotx, 1.0, 0.0, 0.0);
  glRotatef(view_roty, 0.0, 1.0, 0.0);
  glRotatef(view_rotz, 0.0, 0.0, 1.0);

  draw_gearGLES1(state_gear1(), -3.0, -2.0, state_angle());
  draw_gearGLES1(state_gear2(), 3.1, -2.0, -2.0 * state_angle() - 9.0);
  draw_gearGLES1(state_gear3(), -3.1, 4.2, -2.0 * state_angle() - 25.0);

  glPopMatrix();
}

static void init_scene_GLES1()
{
  const GLfloat light_pos[4] = {5.0, 5.0, 10.0, 1.0};


  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glShadeModel(GL_SMOOTH);

  // vertex and normal array will always be used so do it here once  
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // setup overall texture environment
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   
  glEnable(GL_TEXTURE_2D);
  // setup blend mode for current bound texture unit
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

}

/***********************************************************
 * Name: init_model_proj
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Sets the OpenGL|ES model to default values
 *
 * Returns: void
 *
 ***********************************************************/
static void init_model_projGLES1(void)
{
   // near clipping plane
   const float nearp = 1.0f;
   // far clipping plane
   const float farp = 100.0f;
   float hht;
   float hwd;


   glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   hht = nearp * (float)tan(45.0 / 2.0 / 180.0 * M_PI);
   hwd = hht * (float)window_screen_width() / (float)window_screen_height();
   // set up the viewing frustum
   glFrustumf(-hwd, hwd, -hht, hht, nearp, farp);

   glMatrixMode(GL_MODELVIEW);

}

