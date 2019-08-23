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
#include <stdlib.h>
#include <signal.h>

#include "gles3.h"

#include "user_options.h"
#include "gpu_texture.h"
#include "demo_state.h"
#include "image.h"
#include "camera.h"
#include "print_info.h"
#include "scene.h"
#include "font.h"
#include "test_quad.h"
#include "text.h"
#include "exit.h"
#include "key_input.h"
#include "window.h"
#include "window_manager.h"

extern IMAGE_T rpi_image;

#define FPS_Y 980
#define FPS_X 60

static const char ver_text[] = "RPIGears ver: 1.0 GLES2.0";
static const char fps_text[] = "FPS:";
static int fps_start; 
static short text_id;

static void init_textures(void)
{
   // load a texture buffer
   printf("creating Textures\n");
   int tex = GPU_texture_create(rpi_image.width, rpi_image.height, GPU_RGB8, rpi_image.pixel_data);
   //GPU_texture_mipmap(tex);
   update_tex(tex);
}

void toggle_useVSync(void)
{
  const int sync = options_useVSync() ? 0 : 1;
  update_useVSync(sync);
  window_update_VSync(sync);
  printf("\nvertical sync is %s\n", sync ? "on": "off");
}

static void update_fps(void)
{
  char *fps_str = demo_state_has_fps();
  
  if (fps_str) {
    text_set_start(text_id, fps_start);
    text_add(text_id, FPS_X, FPS_Y, fps_str);
  }
}

static void run_gears(void)
{
  // keep doing the loop while no exit keys hit and exit timer not finished
  while (!exit_is_now())
  {
	  WM_frameClear();
    demo_state_next_frame();
    WM_update();
    update_gear_rotation();
    if (!WM_minimized()) {
      test_quad_draw();
      update_fps();
      text_draw(text_id);
      scene_draw();
    }
    WM_frameEnd();
  }
}

//==============================================================================
static void init_options(int argc, char *argv[])
{
  if (! setup_user_options(argc, argv)) {
    print_CLoptions_help();
  }

  update_timeToRun(options_timeToRun());
  update_angleVel(options_angleVel());
}

void sig_handler(int signo)
{
  printf("CTRL-C\n");
  fflush(0);
  exit(signo);
}

int main (int argc, char *argv[])
{
  signal(SIGINT, sig_handler);
  demo_state_init();
  init_options(argc, argv);
  // Start OGLES
  window_manager_init();
  window_update_VSync(options_useVSync());
  exit_init(state_timeToRun());
  print_info_init();
  key_add_action('v', toggle_useVSync, "toggle vertical sync on/off");
  
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
  
  text_id = text_create();
  text_add(text_id, 0, 0, ver_text);
  text_add(text_id, 0, FPS_Y, fps_text);
  fps_start = text_start(text_id);
  // animate the gears
  run_gears();
  
  printf("\nRPIGears finished\n");
  
  return 0;
}