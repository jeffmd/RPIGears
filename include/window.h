/*
* window.h
*/

#ifndef _WINDOW_H_
  #define _WINDOW_H_
  
  int window_major(void);
  int window_minor(void);
  int window_screen_width(void);
  int window_screen_height(void);
  void window_pos(const int x, const int y);
  void window_show(void);
  void window_hide(void);
  void window_size(const int width, const int height);
  void window_init(void);
  void window_swap_buffers(void);
  void window_release(void);
  void window_update_VSync(const int sync);
  void window_snapshot(const int width, const int height, void *buffer);
  void window_ui_viewport(short pos[2], short size[2], short vis_pos[4]);
  void window_viewport_reset(void);
  int window_inFocus(void);

#endif
