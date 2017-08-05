/*
* scene.c
*/

static void draw_scene(void)
{
  if (options->useGLES2) {
    draw_sceneGLES2();
  }
  else {
    draw_sceneGLES1();
  }
}

static void init_scene(void)
{
  // setup the scene based on rendering mode
  if (options->useGLES2) {
   init_scene_GLES2();
   // Setup the model projection/world
   init_model_projGLES2();
  }
  else { // using gles1
   init_scene_GLES1();
   // Setup the model projection/world
   init_model_projGLES1();
  }
}

