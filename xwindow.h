/*
* xwindow.h
*/

#ifndef _XWINDOW_H_
#define _XWINDOW_H_

  void xwindow_init(const uint width, const uint height);
  void xwindow_close(void);
  void xwindow_check_events(void);
  void xwindow_frame_update(void);
  int xwindow_minimized(void);

#endif
