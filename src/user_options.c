/*
* user_options.c
*/

#include <stdio.h>
#include <string.h>

#include "gles3.h"

typedef struct {

  int useVBO;
  int useVSync;
  int wantInfo;
  GLenum drawMode;
  GLuint timeToRun;
  GLfloat angleVel;
  
} OPTIONS_T;


static OPTIONS_T _options;
static OPTIONS_T * const options = &_options;

int User_Options_useVBO(void)
{
  return options->useVBO;  
}

int User_Options_useVSync(void)
{
  return options->useVSync;  
}

int User_Options_wantInfo(void)
{
  return options->wantInfo;  
}

GLenum User_Options_drawMode(void)
{
  return options->drawMode;
}

GLfloat User_Options_angleVel(void)
{
  return options->angleVel;
}

GLuint User_Options_timeToRun(void)
{
  return options->timeToRun;
}

void User_Options_update_useVSync(const int sync)
{
  options->useVSync = sync;
}

void User_Options_toggle_drawmode(void)
{
  char *modestr = 0;
  
  switch (options->drawMode) {
    case GL_TRIANGLES:
      options->drawMode = GL_LINE_STRIP;
      modestr = "GL_LINE_STRIP";
      break;
      
    case GL_LINE_STRIP:
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

static void user_options_init(void)
{

}

int User_Options_setup(int argc, char *argv[])
{
  int i, optionsgood = 1;

  options->useVBO = 0;
  options->drawMode = GL_TRIANGLES;
  options->timeToRun = 0;
  options->angleVel = -50.0f;

  for ( i=1; i<argc; i++ ) {
    if (strcmp(argv[i], "-info")==0) {
	  options->wantInfo = 1;
    }
    else if ( strcmp(argv[i], "-exit")==0) {
      options->timeToRun = 30000;
      printf("Auto Exit after %i seconds.\n", options->timeToRun/1000 );
    }
    else if ( strcmp(argv[i], "-vsync")==0) {
      // want vertical sync
      options->useVSync = 1;
    }
    else if ( strcmp(argv[i], "-vbo")==0) {
	  // use VBO instead of Vertex Array
	  options->useVBO = 1;
	}
    else if ( strcmp(argv[i], "-line")==0) {
	  // use line mode draw ie wire mesh
	  options->drawMode = GL_LINES;
	}
    else if ( strcmp(argv[i], "-nospin")==0) {
	  // gears don't spin
	  options->angleVel = 0.0f;
	}
    else {
	  printf("\nunknown option: %s\n", argv[i]);
      optionsgood = 0;
    }
  }
  
  user_options_init();
 
  return optionsgood;  
}
