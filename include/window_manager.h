// window_manager.h

#ifndef _WINDOW_MANAGER_H_
  #define _WINDOW_MANAGER_H_
  
  void window_manager_init(void);
  void WM_frameClear(void);
  void WM_frameEnd(void);
  void WM_update(void);
  int WM_minimized(void);

#endif