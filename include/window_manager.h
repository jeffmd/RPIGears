// window_manager.h

#ifndef _WINDOW_MANAGER_H_
  #define _WINDOW_MANAGER_H_
  
  typedef void (* Action)(void);

  void window_manager_init(void);
  int WM_minimized(void);
  void WM_refresh(void);
  void WM_set_draw(Action fn);
  char *WM_has_fps(void);
  float WM_period_rate(void);
#endif
