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

#include "user_options.h"
static OPTIONS_T _options, *options = &_options;

#include "gear.h"
#include "demo_state.h"
static DEMO_STATE_T _state, *state = &_state;

#include "window.h"
static WINDOW_T _window, *window = &_window;

#include "RPi_Logo256.c"

#include "shaders.c"

#include "demo_state.c"

#include "window.c"

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
   window->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(window->display!=EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(window->display, &window->major, &window->minor);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(window->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   // bind the gles api to this thread - this is default so not required
   result = eglBindAPI(EGL_OPENGL_ES_API);
   assert(EGL_FALSE != result);

   // create an EGL rendering context
   // select es 1.x or 2.x based on user option
   context_attributes[1] = 1;
   window->contextGLES1 = eglCreateContext(window->display, config, EGL_NO_CONTEXT, context_attributes);
   assert(window->contextGLES1 != EGL_NO_CONTEXT);

   context_attributes[1] = 2;
   window->contextGLES2 = eglCreateContext(window->display, config, EGL_NO_CONTEXT, context_attributes);
   assert(window->contextGLES2 != EGL_NO_CONTEXT);

   // create an EGL window surface based on current screen size
   success = graphics_get_display_size(0 /* LCD */, &window->screen_width, &window->screen_height);
   assert( success >= 0 );

   window->dst_rect.x = window->screen_width/4;
   window->dst_rect.y = window->screen_height/4;
   window->dst_rect.width = window->screen_width/2;
   window->dst_rect.height = window->screen_height/2;

   window->src_rect.x = 0;
   window->src_rect.y = 0;
   window->src_rect.width = (window->screen_width/2) << 16;
   window->src_rect.height = (window->screen_height/2) << 16;

   window->dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   
   dispman_update = vc_dispmanx_update_start( 0 );

   window->dispman_element = vc_dispmanx_element_add( dispman_update, window->dispman_display,
      0/*layer*/, &window->dst_rect, 0/*src*/,
      &window->src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);

   vc_dispmanx_update_submit_sync( dispman_update );

   window->nativewindow.element = window->dispman_element;
   window->nativewindow.width = window->screen_width;
   window->nativewindow.height = window->screen_height;

   window->surface = eglCreateWindowSurface( window->display, config, &window->nativewindow, NULL );
   assert(window->surface != EGL_NO_SURFACE);

   // connect the context to the surface
   result = eglMakeCurrent(window->display, window->surface, window->surface, options->useGLES2 ? window->contextGLES2 : window->contextGLES1);
   assert(EGL_FALSE != result);

   // default to no vertical sync but user option may turn it on
   update_useVSync(options->useVSync);

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
  if (options->useGLES2) {
    draw_sceneGLES2();
  }
  else {
    draw_sceneGLES1();
  }
}

static void update_Window(void)
{
  if (window->update) {
    check_window_offsets();
    
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);
  
    int result = vc_dispmanx_element_change_attributes(update,
                                            window->dispman_element,
                                            0,
                                            0,
                                            255,
                                            &(window->dst_rect),
                                            &(window->src_rect),
                                            0,
                                            DISPMANX_ROTATE_90);
      assert(result == 0);
  
      result = vc_dispmanx_update_submit(update, 0, 0);
      assert(result == 0);
      window->update = 0;
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
    
    do_key_down_update();
    update_Window();
    inc_move_rate();
    update_gear_rotation();
	  frameClear();
    // draw the scene for the next new frame
    draw_scene();
    // swap the current buffer for the next new frame
    eglSwapBuffers(window->display, window->surface);
  }
}

//------------------------------------------------------------------------------

static void exit_func(void)
// Function to be passed to atexit().
{
  if (!options->useGLES2) {
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
  }
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  
  // clear screen
  frameClear();
  eglSwapBuffers(window->display, window->surface);
  
  // Release OpenGL resources
  eglMakeCurrent( window->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
  eglDestroySurface( window->display, window->surface );
  eglDestroyContext( window->display, window->contextGLES1 );
  eglDestroyContext( window->display, window->contextGLES2 );
  eglTerminate( window->display );
  
  // release memory used for gear and associated vertex arrays
  free_gear(state->gear1);
  free_gear(state->gear2);
  free_gear(state->gear3);
  
  printf("\nRPIGears finished\n");
   
} // exit_func()

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

//==============================================================================

int main (int argc, char *argv[])
{
  bcm_host_init();
  
  init_demo_state();
  setup_user_options(argc, argv);
  
  // Start OGLES
  init_egl();
  if (options->wantInfo) {
   print_GLInfo();
  }
  init_textures();
  build_gears();
  
  init_scene();
  
  // animate the gears
  run_gears();
  
  exit_func();
  
  return 0;
}
