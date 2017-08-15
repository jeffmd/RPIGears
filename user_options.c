/*
* user_options.c
*/

static OPTIONS_T _options, *options = &_options;

static void update_useVSync(const int sync)
{
  options->useVSync = sync;
  EGLBoolean result = eglSwapInterval(window_display(), options->useVSync );
  assert(EGL_FALSE != result);
}

static void toggle_useVSync(void)
{
  int sync = options->useVSync ? 0 : 1;
  update_useVSync(sync);
  printf("\nvertical sync is %s\n", sync ? "on": "off");
}

static void toggle_drawmode(void)
{
  char *modestr = 0;
  
  switch (options->drawMode) {
    case GL_TRIANGLES:
      options->drawMode = GL_LINES;
      modestr = "GL_LINES";
      break;
      
    case GL_LINES:
      options->drawMode = GL_POINTS;
      modestr = "GL_POINTS";
      break;
      
    case GL_POINTS:
      options->drawMode = GL_TRIANGLES;
      modestr = "GL_TRIANGLES";
      break;
  }
  
  printf("\ndraw mode is %s\n", modestr);
}

static void setup_user_options(int argc, char *argv[])
{
  int i, printhelp = 0;

  options->useVBO = 0;
  options->drawMode = GL_TRIANGLES;

  for ( i=1; i<argc; i++ ) {
    if (strcmp(argv[i], "-info")==0) {
	  options->wantInfo = 1;
    }
    else if ( strcmp(argv[i], "-exit")==0) {
      state->timeToRun = 30000;
      printf("Auto Exit after %i seconds.\n", state->timeToRun/1000 );
    }
    else if ( strcmp(argv[i], "-vsync")==0) {
      // want vertical sync
      options->useVSync = 1;
    }
    else if ( strcmp(argv[i], "-vbo")==0) {
	  // use VBO instead of Vertex Array
	  options->useVBO = 1;
	}
    else if ( strcmp(argv[i], "-gles2")==0) {
	  // use opengl es 2.0
	  options->useGLES2 = 1;
	}
    else if ( strcmp(argv[i], "-line")==0) {
	  // use line mode draw ie wire mesh
	  options->drawMode = GL_LINES;
	}
    else if ( strcmp(argv[i], "-nospin")==0) {
	  // gears don't spin
	  state->angleVel = 0.0f;
	}
    else {
	  printf("\nunknown option: %s\n", argv[i]);
      printhelp = 1;
    }
  }

  if (printhelp) {
    print_CLoptions_help();
  }
  
  
}
