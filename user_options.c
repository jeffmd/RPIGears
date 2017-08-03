/*
* user_options.c
*/

static void setup_user_options(int argc, char *argv[])
{
  int i, printhelp = 0;


  for ( i=1; i<argc; i++ ) {
    if (strcmp(argv[i], "-info")==0) {
	  state->wantInfo = 1;
    }
    else if ( strcmp(argv[i], "-exit")==0) {
      state->timeToRun = 30000;
      printf("Auto Exit after %i seconds.\n", state->timeToRun/1000 );
    }
    else if ( strcmp(argv[i], "-vsync")==0) {
      // want vertical sync
      state->useVSync = 1;
      state->avgfps = 60;
    }
    else if ( strcmp(argv[i], "-vbo")==0) {
	  // use VBO instead of Vertex Array
	  state->useVBO = 1;
	}
    else if ( strcmp(argv[i], "-gles2")==0) {
	  // use opengl es 2.0
	  state->useGLES2 = 1;
	}
    else if ( strcmp(argv[i], "-line")==0) {
	  // use line mode draw ie wire mesh
	  state->drawMode = GL_LINES;
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
  
  update_angleFrame();
  
}
