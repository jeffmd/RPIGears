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

static short frame_rate_task;
static short FPS_task;
static short refresh_task;

// Average Frames Per Second
static float avgfps;
// the average time between each frame update = 1/avgfps
static float period_rate;

static Action draw_fn;

static void wm_frame_clear(void)
{
  GPU_framebuffer_done();
  // if main screen is visible
  {
    Window_viewport_reset();
  }
  // else render to offscreen framebuffer 
  {
    // enable offscreen frame buffer
  }
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);
  glStencilMask(0xFFFFFFFF);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

static void window_manager_delete(void)
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  wm_frame_clear();

  Window_swap_buffers();
  Window_release();
  XWindow_close();
  bcm_host_deinit();
  printf("window manager has shut down\n"); 
}

static void wm_frame_end(void)
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
  if (draw_fn) {
    draw_fn();
  }
}

static void wm_frame_update(void)
{
  Key_input_down_update();
  wm_do_draw_fn();
  glEnable(GL_SCISSOR_TEST);
  UI_area_root_draw();
}

static void wm_update_avgfps(const float fps)
{
  if ( fabsf(avgfps - fps) > 0.1f ) {
    avgfps = fps;
    period_rate = 1.0f / avgfps;
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

float WM_fps(void)
{
 
  return avgfps;
}

float WM_period_rate(void)
{
  return period_rate;
}

void WM_set_draw(Action fn)
{
  draw_fn = fn;
}

static void frame_refresh(void)
{
  frames++;
  wm_frame_clear();

  if (!WM_minimized()) {
    wm_frame_update();
  }

  wm_frame_end();

}

static void refresh_task_set_interval(const uint interval)
{
  if (!refresh_task) {
    refresh_task = Task_create(interval, frame_refresh);
  }
  else {
    Task_set_interval(refresh_task, interval);
  }
}

void WM_set_fps(const float fps)
{
  avgfps = fps;
  period_rate = 1.0f / avgfps;
  refresh_task_set_interval(period_rate * 1000);
}

void WM_init(void)
{
  bcm_host_init();
  gles3_init();
  Window_init();
  XWindow_init(Window_screen_width() / 2, Window_screen_height() / 2);
  Key_input_init();
  atexit(window_manager_delete);
  WM_set_fps(50.0f);
  frames = lastFrames = 0;
  
  FPS_task = Task_create(5000, wm_do_FPS_task);
  frame_rate_task = Task_create(100, wm_do_frame_rate_task);
}
