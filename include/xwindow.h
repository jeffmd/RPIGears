/*
* xwindow.h
*/

#ifndef _XWINDOW_H_
#define _XWINDOW_H_

  void XWindow_init(const uint width, const uint height);
  void XWindow_close(void);
  void XWindow_check_events(void);
  void XWindow_frame_update(void *buffer);
  int XWindow_minimized(void);
  int XWindow_is_clear(void); 

#endif
