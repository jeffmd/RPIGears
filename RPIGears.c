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
// three rotating gears rendered with OpenGL ES 2.0.

#define _GNU_SOURCE

#include <stdio.h>

#include "bcm_host.h"

#include "gles3.h"

#include "gear.h"
#include "user_options.h"
#include "gpu_texture.h"
#include "demo_state.h"
#include "window.h"
#include "tasks.h"
#include "image.h"
#include "camera.h"
#include "xwindow.h"
#include "print_info.h"
#include "scene.h"
#include "font.h"
#include "test_quad.h"
#include "text.h"

extern IMAGE_T rpi_image;

const char test_text[] = "RPIGears ver: 1.0 GLES2.0";
static Text *text;

static void init_textures(void)
{
   // load a texture buffer
   printf("creating Textures\n");
   GPUTexture *tex = GPU_texture_create(rpi_image.width, rpi_image.height, GPU_RGB8, rpi_image.pixel_data);
   //GPU_texture_mipmap(tex);
   update_tex(tex);
}

static void frameClear(void)
{
  glDisable(GL_SCISSOR_TEST);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);
  glStencilMask(0xFFFFFFFF);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

}

static void frameEnd(void)
{
  const GLenum attachments[3] = { GL_COLOR_EXT, GL_DEPTH_EXT, GL_STENCIL_EXT };
  glDiscardFramebufferEXT( GL_FRAMEBUFFER , 3, attachments);
}

void toggle_useVSync(void)
{
  const int sync = options_useVSync() ? 0 : 1;
  update_useVSync(sync);
  window_update_VSync(sync);
  printf("\nvertical sync is %s\n", sync ? "on": "off");
}

static void run_gears(void)
{
  
  reset_tasks();
  set_key_down_update(0, 0.0f);
  window_init_size();

  // keep doing the loop while no exit keys hit and exit timer not finished
  while ( ! run_exit_task() )
  {
	  frameClear();
    do_tasks();
    do_key_down_update();
    xwindow_check_events();
    window_update();
    inc_move_rate();
    update_gear_rotation();
    scene_draw();
    test_quad_draw();
    text_draw(text);
    frameEnd();
    window_swap_buffers();
    xwindow_frame_update();
  }
}

//------------------------------------------------------------------------------

static void exit_func(void)
// Function to be passed to atexit().
{

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // clear screen
  frameClear();
  window_swap_buffers();

  // Release OpenGL resources
  window_release();
  demo_state_delete();
  test_quad_delete();
  printf("\nRPIGears finished\n");

} // exit_func()

//==============================================================================
static void init_options(int argc, char *argv[])
{
  if (! setup_user_options(argc, argv)) {
    print_CLoptions_help();
  }

  update_timeToRun(options_timeToRun());
  update_angleVel(options_angleVel());
}

int main (int argc, char *argv[])
{
  bcm_host_init();
  gles3_init();
  demo_state_init();
  init_options(argc, argv);
  // Start OGLES
  window_init();
  xwindow_init(window_screen_width() / 2, window_screen_height() / 2);
  // default to no vertical sync but user option may turn it on
  window_update_VSync(options_useVSync());

  if (options_wantInfo()) {
   print_GLInfo();
  }
  init_textures();
  demo_state_build_gears(options_useVBO());

  camera_init();
  scene_init();
  //font_set_active(font_create("liberation2/LiberationMono-Regular.ttf"));
  //font_set_active(font_create("dejavu/DejaVuSans.ttf"));
  font_set_active(font_create("noto/NotoMono-Regular.ttf"));
  test_quad();
  test_quad_set_texture(font_texture(font_active()));
  
  text = text_create();
  text_add(text, 0, 0, test_text);
  
  // animate the gears
  run_gears();
  
  exit_func();
  xwindow_close();
  bcm_host_deinit();
  return 0;
}
