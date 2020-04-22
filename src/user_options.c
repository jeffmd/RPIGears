/*
* user_options.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gles3.h"
#include "key_input.h"

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

int options_useVBO(void)
{
  return options->useVBO;  
}

int options_useVSync(void)
{
  return options->useVSync;  
}

int options_wantInfo(void)
{
  return options->wantInfo;  
}

GLenum options_drawMode(void)
{
  return options->drawMode;
}

GLfloat options_angleVel(void)
{
  return options->angleVel;
}

GLuint options_timeToRun(void)
{
  return options->timeToRun;
}

void update_useVSync(const int sync)
{
  options->useVSync = sync;
}

void toggle_drawmode(void)
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

static void toggle_drawmode_key(const short souce_id, const short destination_id)
{
  toggle_drawmode();
}

void user_options_init(void)
{
  Key_add_action('l', toggle_drawmode_key, "toggle draw mode: GL_TRIANGLES / GL_LINES / GL_POINT");

}

int setup_user_options(int argc, char *argv[])
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
