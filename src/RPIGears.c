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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "gles3.h"

#include "user_options.h"
#include "user_options_ui.h"
#include "gpu_texture.h"
#include "gpu_framebuffer.h"
#include "demo_state.h"
#include "demo_state_ui.h"
#include "image.h"
#include "print_info.h"
#include "scene.h"
#include "font.h"
#include "test_quad.h"
#include "exit.h"
#include "key_input.h"
#include "window.h"
#include "window_manager.h"
#include "gldebug.h"
#include "tasks.h"
#include "ui_view3d.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "stats_ui.h"

extern IMAGE_T rpi_image;

static short render_tex;
static short offscreen_fb;
static short offscreen_draw;
static short view3d_area;

//static short view3d_area_2;

static void init_textures(void)
{
   // load a texture buffer
   printf("creating Textures\n");
   int tex = GPU_texture_create(rpi_image.width, rpi_image.height, GPU_RGB8, rpi_image.pixel_data);
   //GPU_texture_mipmap(tex);
   DS_update_tex(tex);
}

static void toggle_back_render(const short souce_id, const short destination_id)
{
  offscreen_draw = !offscreen_draw;
}

static void offscreen_refresh(void)
{
  if (offscreen_draw) {
    Window_viewport_reset();
    GPU_framebuffer_bind(offscreen_fb);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    Scene_draw();
  }
}

static void run_gears(void)
{
  // keep doing the loop while no exit keys hit and exit timer not finished
  while (!Exit_is_now())
  {
    offscreen_refresh();
    Task_do();
  }
}

static void draw(void)
{
  DS_update_gear_rotation();
  test_quad_draw();
}

//==============================================================================
static void init_options(int argc, char *argv[])
{
  if (! User_Options_setup(argc, argv)) {
    Print_CLoptions_help();
  }

  DS_update_timeToRun(User_Options_timeToRun());
  DS_update_angleVel(User_Options_angleVel());
}

void sig_handler(int signo)
{
  printf("CTRL-C\n");
  fflush(0);
  exit(signo);
}

static void setup_render_texture(const int width, const int height)
{
  check_gl_error("starting setup frameBufferRenderTexture");
  // Build the texture that will serve as the color attachment for the framebuffer.
  render_tex = GPU_texture_create(width, height, GPU_RGB8, NULL);
  check_gl_error("make color texture buffer");

  // Build the texture that will serve as the depth attachment for the framebuffer.
  const int depth_tex = GPU_texture_create(width, height, GPU_DEPTH24, NULL);
  check_gl_error("make depth texture buffer");
  
  // Build the framebuffer.
  offscreen_fb = GPU_framebuffer_create();
  GPU_framebuffer_texture_attach(offscreen_fb, render_tex);
  GPU_framebuffer_texture_attach(offscreen_fb, depth_tex);
  GPU_framebuffer_bind(offscreen_fb);
  
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
    printf("*** Frame buffer incomplete: %s\n", get_FramebufferStatus_msg(status));
}

static void setup_test_quad(void)
{
  test_quad();
  test_quad_add_texture(render_tex, 0.0f);
  test_quad_add_texture(Font_texture(Font_active()), 0.5f);
}

static short get_view3d_area(void)
{
  if (!view3d_area) {
    view3d_area = UI_area_create();
    UI_area_set_root(view3d_area);
    UI_area_set_offset(view3d_area, 1, 1);
    UI_area_set_size(view3d_area, 600, 500);
  }

  return view3d_area;
}

int main (int argc, char *argv[])
{
  signal(SIGINT, sig_handler);
  init_options(argc, argv);
  // Start OGLES
  WM_init();
  WM_set_draw(draw);
  Window_update_VSync(User_Options_useVSync());
  Exit_init(DS_timeToRun());
  Print_info_init();
  DS_ui_init();
  User_options_ui_init();
  Key_add_action(SHIFT_KEY('B'), toggle_back_render, "toggle background render on/off");
  
  if (User_Options_wantInfo()) {
   Print_GLInfo();
  }
  
  init_textures();

  //Font_set_active(Font_create("liberation2/LiberationMono-Regular.ttf"));
  //Font_set_active(Font_create("dejavu/DejaVuSans.ttf"));
  Font_set_active(Font_create("noto/NotoMono-Regular.ttf"));

  setup_render_texture(128, 128);
  setup_test_quad();

  //view3d_area_2 = UI_area_create();
  //UI_area_add(view3d_area, view3d_area_2);
  //UI_area_set_position(view3d_area_2, 1, 1);
  //UI_area_set_size(view3d_area_2, 300, 100);

  UI_area_connect(get_view3d_area(), UI_view3d_create());
  //UI_area_set_handler(view3d_area_2, UI_view3d_area_handler());

  Stats_ui_set_parent_area(get_view3d_area());

  // animate the gears
  run_gears();
  
  printf("\nRPIGears finished\n");
  
  return 0;
}
