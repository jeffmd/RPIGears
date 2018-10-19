/*
* window.h
*/

#ifndef _WINDOW_H_
  #define _WINDOW_H_
  #include "EGL/egl.h"
  
  int window_major(void);
  int window_minor(void);
  EGLDisplay window_display(void);
  uint32_t window_screen_width(void);
  uint32_t window_screen_height(void);
  void window_move_x(const float val);
  void window_move_y(const float val);
  void window_pos(const int x, const int y);
  void window_show(void);
  void window_hide(void);
  void window_center(void);
  void window_zoom(const float val);
  void window_size(const int width, const int height);
  void window_update(void);
  void window_init(void);
  void window_init_pos(void);
  void window_init_size(void);
  void window_swap_buffers(void);
  void window_release(void);
  void window_update_VSync(const int sync);
  void window_snapshot(const int width, const int height, void *buffer);
  int window_inFocus(void);
  void window_noupdate(void);

#endif
