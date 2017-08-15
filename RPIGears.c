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
#define _GNU_SOURCE
#include <math.h>
#include <assert.h>
#include <termio.h>
#include <sys/time.h>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "matrix_math.h"

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

#include "demo_state.h"
static DEMO_STATE_T _state, *state = &_state;

#include "window.h"

#include "RPi_Logo256.c"

#include "shaders.c"

#include "demo_state.c"


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

#include "gear.h"

#include "gles1.c"

#include "gles2.c"

#include "scene.c"

#include "key_input.c"

#include "tasks.c"

static void build_gears(const int useVBO)
{
  const GLfloat red[4] = {0.9, 0.3, 0.3, 1.0};
  const GLfloat green[4] = {0.3, 0.9, 0.3, 1.0};
  const GLfloat blue[4] = {0.3, 0.3, 0.9, 1.0};

  /* make the meshes for the gears */
  state->gear1 = gear(1.0, 4.0, 2.5, 20, 0.7, red);
  state->gear2 = gear(0.5, 2.0, 3.0, 10, 0.7, green);
  state->gear3 = gear(1.3, 2.0, 1.5, 10, 0.7, blue);
  
  // if VBO enabled then set them up for each gear
  if (useVBO) {
    make_gear_vbo(state->gear1);
    make_gear_vbo(state->gear2);
    make_gear_vbo(state->gear3);
  }
  else {
    set_gear_va_ptrs(state->gear1);   
    set_gear_va_ptrs(state->gear2);   
    set_gear_va_ptrs(state->gear3);   
  }

}


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
    draw_scene();
    window_swap_buffers();
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
  window_swap_buffers();
  
  // Release OpenGL resources
  window_release();
  
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
  
  init_demo_state();
  setup_user_options(argc, argv);
  
  // Start OGLES
  init_window(options->useVSync, options->useGLES2);
  // default to no vertical sync but user option may turn it on
  update_useVSync(options->useVSync);

  if (options->wantInfo) {
   print_GLInfo();
  }
  init_textures();
  build_gears(options->useVBO);
  
  init_scene();
  
  // animate the gears
  run_gears();
  
  exit_func();
  
  return 0;
}
