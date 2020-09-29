// window_manager.h

#ifndef _WINDOW_MANAGER_H_
  #define _WINDOW_MANAGER_H_
  
  typedef void (* Action)(void);

  void WM_init(void);
  int WM_minimized(void);
  void WM_set_draw(Action fn);
  float WM_fps(void);
  void WM_set_fps(const float fps);
  float WM_period_rate(void);
#endif
