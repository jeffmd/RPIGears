/*
* xwindow.h
*/

#ifndef _XWINDOW_H_
#define _XWINDOW_H_

  extern void xwindow_init(const uint width, const uint height);
  extern void xwindow_close(void);
  extern void xwindow_check_events(void);
  extern void xwindow_frame_update(void);

#endif
