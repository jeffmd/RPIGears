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
  extern void move_window_x(const float val);
  extern void move_window_y(const float val);
  extern void move_window_home(void);
  extern void move_window_end(void);
  extern void zoom_window(const float val);
  extern void update_Window(void);
  extern void init_window(const int useVSync, const int useGLES2);
  extern void init_window_pos(void);
  extern void init_window_size(void);
  extern void window_swap_buffers(void);
  extern void window_release(void);
  extern void window_update_VSync(const int sync);

#endif
