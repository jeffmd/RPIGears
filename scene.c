/*
* scene.c
*/

static void draw_scene(void)
{
  draw_sceneGLES2();
}

static void init_scene(void)
{
  glViewport(0, 0, (GLsizei)window_screen_width(), (GLsizei)window_screen_height());
  
  // setup the scene based on rendering mode
  init_ProjectionMatrix((float)window_screen_width() / (float)window_screen_height());
  init_scene_GLES2();
}

