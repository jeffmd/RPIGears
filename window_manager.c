// window_manage.c

#include "bcm_host.h"
#include "gles3.h"
#include "window.h"
#include "xwindow.h"
#include "key_input.h"
#include "tasks.h"

void window_manager_init(void)
{
  bcm_host_init();
  gles3_init();
  window_init();
  xwindow_init(window_screen_width() / 2, window_screen_height() / 2);
  key_input_init();
}

void WM_frameClear(void)
{
  glDisable(GL_SCISSOR_TEST);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);
  glStencilMask(0xFFFFFFFF);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void window_manager_delete(void)
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  WM_frameClear();

  window_swap_buffers();
  window_release();
  xwindow_close();
  bcm_host_deinit();
  
}

void WM_frameEnd(void)
{
  const GLenum attachments[3] = { GL_COLOR_EXT, GL_DEPTH_EXT, GL_STENCIL_EXT };
  glDiscardFramebufferEXT( GL_FRAMEBUFFER , 3, attachments);
  window_swap_buffers();
  xwindow_frame_update();
}

void WM_update(void)
{
  do_tasks();
  xwindow_check_events();
  key_input_down_update();
  key_input_inc_rate();
  if (xwindow_is_clear())
    window_update();
}

int WM_minimized(void)
{
  return xwindow_minimized();
}

