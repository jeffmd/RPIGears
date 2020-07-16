// window_manage.c

#include <stdio.h>
#include <math.h>

#include "bcm_host.h"
#include "gles3.h"
#include "gpu_framebuffer.h"
#include "gpu_texture.h"
#include "window.h"
#include "xwindow.h"
#include "key_input.h"
#include "tasks.h"
#include "ui_area_action.h"

typedef void (* Action)(void);

static int frames; // number of frames drawn since the last frame/sec calculation
static int lastFrames;
static char fps_str[12];
static char *fps_strptr;

static short frame_rate_task;
static short FPS_task;

// Average Frames Per Second
static float avgfps;
// the average time between each frame update = 1/avgfps
static float period_rate;

static Action draw_fn;

static void wm_frameClear(void)
{
  GPU_framebuffer_done();
  // if main screen is visible
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, (GLsizei)Window_screen_width(), (GLsizei)Window_screen_height());
  }
  // else render to offscreen framebuffer 
  {
    // enable offscreen frame buffer
  }
  glDisable(GL_SCISSOR_TEST);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);
  glStencilMask(0xFFFFFFFF);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

static void window_manager_delete(void)
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  wm_frameClear();

  Window_swap_buffers();
  Window_release();
  XWindow_close();
  bcm_host_deinit();
  printf("window manager has shut down\n"); 
}

static void wm_frameEnd(void)
{
  Window_swap_buffers();
  XWindow_frame_update(0);
}

int WM_minimized(void)
{
  return XWindow_minimized();
}

static void wm_do_draw_fn(void)
{
  if (!WM_minimized()) {
    if (draw_fn) {
      draw_fn();
    }
  }
}

static void wm_update(void)
{
  Task_do();
  XWindow_check_events();
  Key_input_down_update();
  Key_input_inc_rate();
  wm_do_draw_fn();
  glEnable(GL_SCISSOR_TEST);
  UI_area_root_draw();
  
}

static void wm_update_avgfps(const float fps)
{
  if ( fabsf(avgfps - fps) > 0.1f ) {
    sprintf(fps_str, "%3.1f  ", fps);
    fps_strptr = fps_str;
    avgfps = fps;
    period_rate = 1.0f / avgfps;
    //update_angleFrame();
    Key_input_set_rate_frame(period_rate);
  }
}

static void wm_do_frame_rate_task(void)
{
  const float dt = Task_elapsed(frame_rate_task) / 1000.0f;
  if (dt > 0.0f) {
	  
    //printf("dt: %f\n", dt);
    //printf("frames: %i\n", frames - lastFrames);
    wm_update_avgfps((float)(frames - lastFrames) / dt);
    lastFrames = frames;
  }
}

static void wm_do_FPS_task(void)
{
  const float dt = Task_elapsed(FPS_task) / 1000.0f;
  const float fps = (float)frames / dt;

  printf("%d frames in %3.1f seconds = %3.1f FPS\n", frames, dt, fps);
  lastFrames = lastFrames - frames;
  frames = 0;
}

char *WM_has_fps(void)
{
  char * str = fps_strptr;
  fps_strptr = 0;
  
  return str;
}

float WM_period_rate(void)
{
  return period_rate;
}

void WM_set_draw(Action fn)
{
  draw_fn = fn;
}

void WM_refresh(void)
{
  frames++;
  wm_frameClear();
  wm_update();
  wm_frameEnd();
}

void WM_init(void)
{
  bcm_host_init();
  gles3_init();
  Window_init();
  XWindow_init(Window_screen_width() / 2, Window_screen_height() / 2);
  Key_input_init();
  atexit(window_manager_delete);
  avgfps = 50.0f;
  period_rate = 1.0f / avgfps;
  
  frames = lastFrames = 0;
  
  FPS_task = Task_create(5000, wm_do_FPS_task);
  frame_rate_task = Task_create(100, wm_do_frame_rate_task);
}
