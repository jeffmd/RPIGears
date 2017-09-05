/*
* xwindow.c
*/

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
//#include <X11/Xos.h>
#include <assert.h>

#include "xinput.h"


static Display *dis;
static int screen;
static Window win;
//static GC gc;

void init_xwindow(const uint width, const uint height)
{
	/* get the colors black and white (see section for details) */
	unsigned long black, white;

	/* use the information from the environment variable DISPLAY 
	   to create the X connection:
	*/	
	dis = XOpenDisplay((char *)0);
  assert(dis);
  screen = DefaultScreen(dis);
	black = BlackPixel(dis, screen),	/* get color black */
	white = WhitePixel(dis, screen);  /* get color white */

	/* once the display is initialized, create the window.
	   This window will be have be 200 pixels across and 300 down.
	   It will have the foreground white and background black
	*/
  win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0,	
    width, height, 0, white, black);

	/* here is where some properties of the window can be set.
	   The third and fourth items indicate the name which appears
	   at the top of the window and the name of the minimized window
	   respectively.
	*/
	XSetStandardProperties(dis, win, "RPIGears", "Demo", None, NULL, 0, NULL);

	/* this routine determines which types of input are allowed in
	   the input.  see the appropriate section for details...
	*/
	XSelectInput(dis, win, 
      ExposureMask
    | ButtonPressMask
    | Button1MotionMask
    | KeyPressMask
    /*| SubstructureRedirectMask */
    /*| FocusChangeMask*/
    /*| EnterWindowMask*/
    /*| PointerMotionMask*/
    /*| KeyReleaseMask*/
    | StructureNotifyMask);

	/* create the Graphics Context */
  //gc = XCreateGC(dis, win, 0,0);        

	//XSetBackground(dis, gc, white);
	//XSetForeground(dis, gc, black);
  
  Atom wmprotocols[2] = {
    // prevent xwindow from closing on its own
    XInternAtom(dis, "WM_DELETE_WINDOW", False), 
    XInternAtom(dis, "WM_TAKE_FOCUS", False)
  };
  
  XSetWMProtocols(dis, win, wmprotocols, 2);

	XClearWindow(dis, win);
	XMapRaised(dis, win);
  //XFlush(dis);
}

void xwindow_close(void)
{
  //XFreeGC(dis, gc);
  XDestroyWindow(dis,win);
  XCloseDisplay(dis);	
}

void xwindow_check_events(void)
{
  XEvent event;
  KeySym key;
    
  int cnt = XPending(dis);
  if (cnt > 0) {
    //printf("xevents pending: %i\n", cnt);
    XNextEvent(dis, &event);
    /* keyboard events */
    if (event.type == KeyPress)
    {
      key = XLookupKeysym(&event.xkey, 0);
      //printf( "KeyPress: %x\n", (int)key );
      x_process_keypress(key);

    }
    else if (event.type == KeyRelease)
    {
      //printf( "KeyRelease: %x\n", event.xkey.keycode );
    }
  }
}
