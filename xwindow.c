/*
* xwindow.c
*/

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <assert.h>


static Display *dis;
static int screen;
static Window win;
static GC gc;

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
	XSetStandardProperties(dis, win, "RPIGears", "Demo", None, NULL,0, NULL);

	/* this routine determines which types of input are allowed in
	   the input.  see the appropriate section for details...
	*/
	XSelectInput(dis, win, ExposureMask | ButtonPressMask | KeyPressMask /*| KeyReleaseMask*/);

	/* create the Graphics Context */
  gc = XCreateGC(dis, win, 0,0);        

	/* here is another routine to set the foreground and background
	   colors _currently_ in use in the window.
	*/
	XSetBackground(dis, gc, white);
	XSetForeground(dis, gc, black);

	/* clear the window and bring it on top of the other windows */
	XClearWindow(dis, win);
	XMapRaised(dis, win);
  XFlush(dis);
}

void xwindow_close(void)
{
  XFreeGC(dis, gc);
  XDestroyWindow(dis,win);
  XCloseDisplay(dis);	
}

void xwindow_check_events(void)
{
  XEvent event;
  int cnt = XPending(dis);
  if (cnt > 0) {
    printf("xevents pending: %i\n", cnt);
    XNextEvent(dis, &event);
    /* keyboard events */
    if (event.type == KeyPress)
    {
        printf( "KeyPress: %x\n", event.xkey.keycode );

    }
    else if (event.type == KeyRelease)
    {
        printf( "KeyRelease: %x\n", event.xkey.keycode );
    }
  }
}
