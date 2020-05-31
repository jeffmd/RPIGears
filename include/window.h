/*
* window.h
*/

#ifndef _WINDOW_H_
  #define _WINDOW_H_
  
  int Window_major(void);
  int Window_minor(void);
  int Window_screen_width(void);
  int Window_screen_height(void);
  void Window_pos(const int x, const int y);
  void Window_show(void);
  void Window_hide(void);
  void Window_size(const int width, const int height);
  void Window_init(void);
  void Window_swap_buffers(void);
  void Window_release(void);
  void Window_update_VSync(const int sync);
  void Window_snapshot(const int width, const int height, void *buffer);
  void Window_ui_viewport(short pos[2], short size[2], short vis_pos[4]);
  void Window_viewport_reset(void);
  int Window_inFocus(void);

#endif
