/*
Copyright (c) 2012, Broadcom Europe Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/* $XFree86: xc/programs/glxgears/glxgears.c,v 1.2 2001/04/03 15:56:26 dawes Exp $ */

/*
 * This is a port of the infamous "gears" demo to straight GLX (i.e. no GLUT)
 * Port by Brian Paul  23 March 2001
 *
 * Command line options:
 *    -info      print GL implementation information
 *
 */

/* Conversion to use vertex buffer objects by Michael J. Clark */

/* This a port of glxgears to the Raspberry Pi
 * Port (cut and paste code monkey dude) by Jeff Doyle 18 Jul 2013
 *
 */
// three rotating gears rendered with OpenGL|ES 1.1.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <termio.h>
#include <sys/time.h>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

//Attributes changes flag mask
#define ELEMENT_CHANGE_LAYER          (1<<0)
#define ELEMENT_CHANGE_OPACITY        (1<<1)
#define ELEMENT_CHANGE_DEST_RECT      (1<<2)
#define ELEMENT_CHANGE_SRC_RECT       (1<<3)
#define ELEMENT_CHANGE_MASK_RESOURCE  (1<<4)
#define ELEMENT_CHANGE_TRANSFORM      (1<<5)

// number of frames to draw before checking if a key on the keyboard was hit
#define FRAMES 30
// default angle velocity of the gears
#define ANGLEVEL 70

#define check() assert(glGetError() == 0)

typedef struct {
  GLfloat pos[3];
  GLfloat norm[3];
  GLfloat texCoords[2];
} vertex_t;

typedef struct {
  vertex_t *vertices;
  GLshort *indices;
  GLfloat color[4];
  int nvertices, nindices;
  GLuint vboId; // ID for vertex buffer object
  GLuint iboId; // ID for index buffer object
  
  GLuint tricount; // number of triangles to draw
  GLvoid *vertex_p; // offset or pointer to first vertex
  GLvoid *normal_p; // offset or pointer to first normal
  GLvoid *index_p;  // offset or pointer to first index
  GLvoid *texCoords_p;  // offset or pointer to first texcoord
} gear_t;

typedef void (*UPDATE_KEY_DOWN)(const float);

typedef struct
{
   uint32_t screen_width;
   uint32_t screen_height;
   float move_rate;
   float move_direction;
   float rate_frame;
   UPDATE_KEY_DOWN key_down_update;
// OpenGL|ES objects
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
   GLenum drawMode;
// EGL info
   int major;
   int minor;
// current distance from camera
   GLfloat viewDist;
   GLfloat view_inc;
   GLfloat viewX;
   GLfloat viewY;
// number of seconds to run the demo
   uint timeToRun;
   GLuint texId;

   gear_t *gear1, *gear2, *gear3;
   
// The location of the shader uniforms 
   GLuint ModelViewProjectionMatrix_location,
      ModelViewMatrix_location,
      NormalMatrix_location,
      LightSourcePosition_location,
      MaterialColor_location,
      DiffuseMap_location;
// The projection matrix
   GLfloat ProjectionMatrix[16];
   
// current angle of the gear
   GLfloat angle;
// the degrees that the angle should change each frame
   GLfloat angleFrame;
// the degrees per second the gear should rotate at
   GLfloat angleVel;
// Average Frames Per Second
   float avgfps;
   int useVBO;
   int useGLES2;
   int useVSync;
   int wantInfo;

	 EGL_DISPMANX_WINDOW_T nativewindow;
	 DISPMANX_ELEMENT_HANDLE_T dispman_element;
	 DISPMANX_DISPLAY_HANDLE_T dispman_display;
	 VC_RECT_T dst_rect;
	 VC_RECT_T src_rect;
   float pos_x;
   float pos_y;
   float width;
   float height;
   int window_update;
} DEMO_STATE_T;

static DEMO_STATE_T _state, *state = &_state;

#include "RPi_Logo256.c"

#include "shaders.c"

#include "demo_state.c"

#include "matrix_math.c"


/***********************************************************
 * Name: init_egl
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Sets the display, OpenGL|ES context and screen stuff
 *
 * Returns: void
 *
 ***********************************************************/
static void init_egl(void)
{
   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   DISPMANX_UPDATE_HANDLE_T dispman_update;


   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_DEPTH_SIZE, 16,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };

  static EGLint context_attributes[] = 
   {
      EGL_CONTEXT_CLIENT_VERSION, 1,
      EGL_NONE
   };

   EGLConfig config;

   // get an EGL display connection
   state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(state->display!=EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(state->display, &state->major, &state->minor);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   // bind the gles api to this thread - this is default so not required
   result = eglBindAPI(EGL_OPENGL_ES_API);
   assert(EGL_FALSE != result);

   // create an EGL rendering context
   // select es 1.x or 2.x based on user option
   context_attributes[1] = state->useGLES2 + 1;
   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
   assert(state->context!=EGL_NO_CONTEXT);

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
   assert( success >= 0 );

   state->dst_rect.x = state->screen_width/4;
   state->dst_rect.y = state->screen_height/4;
   state->dst_rect.width = state->screen_width/2;
   state->dst_rect.height = state->screen_height/2;

   state->src_rect.x = 0;
   state->src_rect.y = 0;
   state->src_rect.width = (state->screen_width/2) << 16;
   state->src_rect.height = (state->screen_height/2) << 16;

   state->dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );

   state->dispman_element = vc_dispmanx_element_add( dispman_update, state->dispman_display,
      0/*layer*/, &state->dst_rect, 0/*src*/,
      &state->src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);

   state->nativewindow.element = state->dispman_element;
   state->nativewindow.width = state->screen_width;
   state->nativewindow.height = state->screen_height;
   vc_dispmanx_update_submit_sync( dispman_update );

   state->surface = eglCreateWindowSurface( state->display, config, &state->nativewindow, NULL );
   assert(state->surface != EGL_NO_SURFACE);

   // connect the context to the surface
   result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
   assert(EGL_FALSE != result);

   // default to no vertical sync but user option may turn it on
   update_useVSync(state->useVSync);

   // Set background color and clear buffers
   glClearColor(0.25f, 0.45f, 0.55f, 0.50f);

   // Enable back face culling.
   glEnable(GL_CULL_FACE);
   glFrontFace(GL_CCW);

}



static GLfloat view_rotx = 25.0, view_roty = 30.0, view_rotz = 0.0;

static void init_textures(void)
{
   // load a texture buffer but use them on six OGL|ES texture surfaces
   glGenTextures(1, &state->texId);

   // setup texture
   glBindTexture(GL_TEXTURE_2D, state->texId);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rpi_image.width, rpi_image.height, 0,
                GL_RGB, GL_UNSIGNED_BYTE, rpi_image.pixel_data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   
}

static void frameClear(void)
{
  glDisable(GL_SCISSOR_TEST);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);
  glStencilMask(0xFFFFFFFF);

  const GLenum attachments[3] = { GL_COLOR_EXT, GL_DEPTH_EXT, GL_STENCIL_EXT };
  glDiscardFramebufferEXT( GL_FRAMEBUFFER , 3, attachments);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
}

#include "print_info.c"

#include "user_options.c"

#include "gear.c"

#include "gles1.c"

#include "gles2.c"

static void draw_scene(void)
{
  if (state->useGLES2) {
    draw_sceneGLES2();
  }
  else {
    draw_sceneGLES1();
  }
}

static void update_Window(void)
{
  if (state->window_update) {
    check_window_offsets();
    
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);
  
    int result = vc_dispmanx_element_change_attributes(update,
                                            state->dispman_element,
                                            0,
                                            0,
                                            255,
                                            &(state->dst_rect),
                                            &(state->src_rect),
                                            0,
                                            DISPMANX_ROTATE_90);
      assert(result == 0);
  
      result = vc_dispmanx_update_submit(update, 0, 0);
      assert(result == 0);
      state->window_update = 0;
    }
}

#include "key_input.c"

#include "tasks.c"

static void run_gears(void)
{
  frames = 0;

  reset_tasks();
  state->key_down_update = 0;
  init_window_pos();
  init_window_size();
  
  // keep doing the loop while no exit keys hit and exit timer not finished
  while ( ! run_task(&Exit_task, 0) )
  {
    frames++;
    do_tasks();
    
    if (state->key_down_update) {
      state->key_down_update(state->move_direction * state->rate_frame);
    }
    update_Window();

    update_gear_rotation();
	  frameClear();
    // draw the scene for the next new frame
    draw_scene();
    // swap the current buffer for the next new frame
    eglSwapBuffers(state->display, state->surface);
  }
}

//------------------------------------------------------------------------------

static void exit_func(void)
// Function to be passed to atexit().
{
   if (!state->useGLES2) {
     glDisableClientState(GL_NORMAL_ARRAY);
     glDisableClientState(GL_VERTEX_ARRAY);
   }

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   // clear screen
   frameClear();
   eglSwapBuffers(state->display, state->surface);

   // Release OpenGL resources
   eglMakeCurrent( state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroySurface( state->display, state->surface );
   eglDestroyContext( state->display, state->context );
   eglTerminate( state->display );

   // release memory used for gear and associated vertex arrays
   free_gear(state->gear1);
   free_gear(state->gear2);
   free_gear(state->gear3);
   
   printf("\nRPIGears finished\n");
   
} // exit_func()

//==============================================================================

int main (int argc, char *argv[])
{
   bcm_host_init();

   // Clear application state
   memset( state, 0, sizeof( *state ) );

   setup_user_options(argc, argv);

   // Start OGLES
   init_egl();
   if (state->wantInfo) {
     print_GLInfo();
   }
   init_textures();
   build_gears();
   
   // setup the scene based on rendering mode
   if (state->useGLES2) {
	   init_scene_GLES2();
     // Setup the model projection/world
     init_model_projGLES2();
   }
   else { // using gles1
     init_scene_GLES1();
     // Setup the model projection/world
     init_model_projGLES1();
   }

   // animate the gears
   run_gears();

   exit_func();

   return 0;
}
