// window_manage.c

#include "bcm_host.h"
#include "gles3.h"
#include "window.h"
#include "xwindow.h"
#include "key_input.h"

void window_manager_init(void)
{
  bcm_host_init();
  gles3_init();
  window_init();
  xwindow_init(window_screen_width() / 2, window_screen_height() / 2);
  key_input_init();
}

void window_manager_delete(void)
{
  window_swap_buffers();
  window_release();
  xwindow_close();
  bcm_host_deinit();
  
}
