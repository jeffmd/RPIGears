/*
* scene.c
*/

static void draw_scene(void)
{
  if (options_useGLES2()) {
    draw_sceneGLES2();
  }
  else {
    draw_sceneGLES1();
  }
}

static void init_scene(void)
{
  glViewport(0, 0, (GLsizei)window_screen_width(), (GLsizei)window_screen_height());
  
  // setup the scene based on rendering mode
  if (options_useGLES2()) {
   init_scene_GLES2();
   init_model_projGLES2();
  }
  else { // using gles1
   init_scene_GLES1();
   init_model_projGLES1();
  }
}

