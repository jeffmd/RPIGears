/*
* window.h
*/

#ifndef _WINDOW_H_
  #define _WINDOW_H_
  #include "EGL/egl.h"
  
  extern int window_major(void);
  extern int window_minor(void);
  extern EGLDisplay window_display(void);
  extern uint32_t window_screen_width(void);
  extern uint32_t window_screen_height(void);
  extern void window_move_x(const float val);
  extern void window_move_y(const float val);
  extern void window_pos(const int x, const int y);
  extern void window_show(void);
  extern void window_hide(void);
  extern void window_center(void);
  extern void window_zoom(const float val);
  extern void window_size(const int width, const int height);
  extern void window_update(void);
  extern void window_init(const int useGLES2);
  extern void window_init_pos(void);
  extern void window_init_size(void);
  extern void window_swap_buffers(void);
  extern void window_release(void);
  extern void window_update_VSync(const int sync);
  extern void window_snapshot(const int width, const int height, void *buffer);
  extern int window_inFocus(void);
  extern void window_noupdate(void);

#endif
