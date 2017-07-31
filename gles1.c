/*
* gles1.c
*/

void draw_gearGLES1(gear_t* gear, GLfloat x, GLfloat y, GLfloat angle)
{

  glPushMatrix();
  glTranslatef(x, y, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);
	
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, gear->color);
  
  if (state->useVBO) {
	glBindBuffer(GL_ARRAY_BUFFER, gear->vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gear->iboId);
  }
  
  glNormalPointer(GL_FLOAT, sizeof(vertex_t), gear->normal_p);
  glVertexPointer(3, GL_FLOAT, sizeof(vertex_t), gear->vertex_p);
  glTexCoordPointer(2, GL_FLOAT, sizeof(vertex_t), gear->texCoords_p);

  // Bind texture surface to current vertices
  glBindTexture(GL_TEXTURE_2D, state->texId);
    
  glDrawElements(state->drawMode, gear->tricount, GL_UNSIGNED_SHORT,
                   gear->index_p);
  glPopMatrix();
  
}

static void draw_sceneGLES1(void)
{
  glPushMatrix();

  glTranslatef(state->viewX, state->viewY, state->viewDist);

  glRotatef(view_rotx, 1.0, 0.0, 0.0);
  glRotatef(view_roty, 0.0, 1.0, 0.0);
  glRotatef(view_rotz, 0.0, 0.0, 1.0);

  draw_gearGLES1(state->gear1, -3.0, -2.0, state->angle);
  draw_gearGLES1(state->gear2, 3.1, -2.0, -2.0 * state->angle - 9.0);
  draw_gearGLES1(state->gear3, -3.1, 4.2, -2.0 * state->angle - 25.0);

  glPopMatrix();
}

static void init_scene_GLES1()
{
  const GLfloat light_pos[4] = {5.0, 5.0, 10.0, 1.0};


  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

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

   glViewport(0, 0, (GLsizei)state->screen_width, (GLsizei)state->screen_height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   hht = nearp * (float)tan(45.0 / 2.0 / 180.0 * M_PI);
   hwd = hht * (float)state->screen_width / (float)state->screen_height;
   // set up the viewing frustum
   glFrustumf(-hwd, hwd, -hht, hht, nearp, farp);

   glMatrixMode(GL_MODELVIEW);

}

