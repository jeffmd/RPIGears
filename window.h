/*
* window.h
*/

typedef  struct {
   // window data
   uint32_t screen_width;
   uint32_t screen_height;
// OpenGL|ES objects
   EGLDisplay display;
   EGLSurface surface;
   EGLContext contextGLES1;
   EGLContext contextGLES2;
// EGL info
   int major;
   int minor;
	 EGL_DISPMANX_WINDOW_T nativewindow;
	 DISPMANX_ELEMENT_HANDLE_T dispman_element;
	 DISPMANX_DISPLAY_HANDLE_T dispman_display;
	 VC_RECT_T dst_rect;
	 VC_RECT_T src_rect;
   float pos_x;
   float pos_y;
   float width;
   float height;
   int update; // if > 0 then window needs updating in dispmanx
} WINDOW_T;


