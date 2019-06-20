/*
* xwindow.c
*/

#include <stdio.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <assert.h>

#include "xinput.h"
#include "window.h"
#include "exit.h"

static Display *dis;
static int screen;
static Window win;
static Atom wmprotocols[3];

static GC gc;
static XImage *img;

static int resized = 1;
static int useMITSHM = 0;
static int minimized = 0;

static XWindowAttributes window_attributes;
static XShmSegmentInfo shmSInfo;


static int checkShmSupport(void)
{
  int major, minor, result;
  Bool pixmapsupport;

  if((result = XShmQueryVersion(dis, &major, &minor, &pixmapsupport))) {
    printf("X11 MIT-SHM support ver:%i.%i\n", major, minor);
  }
  else {
    printf(" no X11 MIT-SHM support");
  }

  return result;
}

static void init_XShmImageBuffer(int width, int height, int depth)
{
  img = XShmCreateImage(dis, CopyFromParent, depth, ZPixmap, NULL, &shmSInfo, width, height);

  shmSInfo.shmid = shmget(IPC_PRIVATE, img->bytes_per_line * img->height, IPC_CREAT|0777);
  shmSInfo.shmaddr = img->data = shmat(shmSInfo.shmid, 0, 0);
  assert(img->data != 0);
  shmSInfo.readOnly = True; // False; faster rendering if readOnly = True
  XShmAttach(dis, &shmSInfo);
}

void xwindow_init(const uint width, const uint height)
{
  /* get the colors black and white (see section for details) */
  unsigned long black, white;


  /* use the information from the environment variable DISPLAY
     to create the X connection:
  */
  dis = XOpenDisplay((char *)0);
  assert(dis);
  screen = DefaultScreen(dis);
  black = BlackPixel(dis, screen),  /* get color black */
  white = WhitePixel(dis, screen);  /* get color white */

  /* once the display is initialized, create the window.
     This window will be have be 200 pixels across and 300 down.
     It will have the foreground white and background black
  */
  win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0,
    width, height, 0, white, black);

  XGetWindowAttributes(dis, win, &window_attributes);
  printf("Window Location: %i,%i \n Window Dimensions %i x %i \n Bit depth : %i \n",
          window_attributes.x,
          window_attributes.y,
          window_attributes.width,
          window_attributes.height,
          window_attributes.depth
          );

  gc = XCreateGC(dis, win, 0, 0/*&gcvalues*/);

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
    |  ButtonPressMask
    | Button1MotionMask
    | KeyPressMask
    /*| SubstructureRedirectMask*/
    | FocusChangeMask
    /*| EnterWindowMask*/
    /*| PointerMotionMask*/
    | KeyReleaseMask
    | VisibilityChangeMask
    | StructureNotifyMask);

  /* create the Graphics Context */
  //gc = XCreateGC(dis, win, 0,0);

  //XSetBackground(dis, gc, white);
  //XSetForeground(dis, gc, black);

  // prevent xwindow from closing on its own
  wmprotocols[0] = XInternAtom(dis, "WM_PROTOCOLS", False);
  wmprotocols[1] = XInternAtom(dis, "WM_DELETE_WINDOW", False);
  wmprotocols[2] = XInternAtom(dis, "WM_TAKE_FOCUS", False);

  XSetWMProtocols(dis, win, &wmprotocols[1], 2);

  useMITSHM = checkShmSupport();
  if(useMITSHM) {
    init_XShmImageBuffer(1280, 1024, 24);
  }

  XClearWindow(dis, win);
  XMapRaised(dis, win);
  //XFlush(dis);
}

void xwindow_close(void)
{
  XFreeGC(dis, gc);
  XDestroyWindow(dis, win);
  if(useMITSHM) {
    XDestroyImage(img);
    XShmDetach(dis, &shmSInfo);
  }
  else {
    XDestroyImage(img);
  }
  XCloseDisplay(dis);
}

static void do_ConfigureNotify(const XConfigureEvent* event)
{
  printf("x:%i y:%i w:%i h:%i\n",
  event->x,
  event->y,
  event->width,
  event->height
  );

  if (event->above==0)
  {
    window_pos(event->x, event->y);
    printf("positioning Window\n");
  }
  else
  {
    window_size(event->width, event->height);
    resized = 1;
    printf("resizing Window\n");
  }
  printf("window: %lu\n", event->above);
  
}

static void do_ClientMessage(const XClientMessageEvent* event)
{
  if (event->message_type == wmprotocols[0])
  {
    if ((Atom)event->data.l[0] ==  wmprotocols[1])
    {
      exit_enable();
    }
    else if ((Atom)event->data.l[0] ==  wmprotocols[2])
    {
      printf("take focus event \n");
      //window_show();
    }
    else {
      printf("unhandled wmprotocol event \n");
    }

  }
  else {
    printf("unhandled client message event \n");
  }
}

#define BYTESPIXEL 4

static inline void byteBufferSwap(char *bottomBuffer, char *topBuffer)
{
  const char tmpval = *bottomBuffer;
  *bottomBuffer = *topBuffer;
  *topBuffer = tmpval;
}

static inline void moveBufferLineFlipRGB(char *bottomBuffer, char *topBuffer, const int stride)
{
  const char * maxbuffer = &bottomBuffer[stride];

  while ( bottomBuffer < maxbuffer) {
    byteBufferSwap(bottomBuffer, &topBuffer[2]);
    byteBufferSwap(&bottomBuffer[2], topBuffer);
    byteBufferSwap(&bottomBuffer[1], &topBuffer[1]);

      bottomBuffer += BYTESPIXEL;
      topBuffer += BYTESPIXEL;
  }
}

static inline void bufferLineFlipRGB(char *buffer, const int stride)
{
  const char * maxbuffer = &buffer[stride];

  while ( buffer < maxbuffer) {
    byteBufferSwap(buffer, &buffer[2]);
      buffer += BYTESPIXEL;
  }
}

static void buffer_flip_vertical(const int stride, const int height, char *buffer)
{
  char *topBuffer = &buffer[(height - 1) * stride];

  while(topBuffer > buffer)
  {
   moveBufferLineFlipRGB(buffer, topBuffer, stride);
   buffer += stride;
   topBuffer -= stride;
  }
  if(topBuffer == buffer) {
    bufferLineFlipRGB(buffer, stride);
  }
}

#if 0
static void init_XimageBuffer(int width, int height, int depth)
{
  img = XCreateImage(dis, CopyFromParent, depth, ZPixmap, 0, NULL, width, height, 32, 0);
  img->data = malloc(height * width * BYTESPIXEL);
  assert(img->data != 0);
  resized = 1;
}
#endif

void xwindow_frame_update(void)
{
  static int dirty = 0;
  
  if(window_inFocus() | minimized) {
    if(dirty) {
      XClearWindow(dis, win);
      dirty = 0;
    }
    
    return;
  }
  
  dirty = 1;
  
  XGetWindowAttributes(dis, win, &window_attributes);
  const int width = window_attributes.width;
  const int height = window_attributes.height;
  //const int depth = window_attributes.depth;

  //if(useMITSHM) {
    if(resized) {
      img->width = width;
      img->height = height;
      img->bytes_per_line = width * BYTESPIXEL;
      resized = 0;
    }
    window_snapshot(width, height, img->data);
    buffer_flip_vertical(img->bytes_per_line, height, img->data);
    XShmPutImage(dis, win, gc, img, 0, 0, 0, 0, width, height, False);
  //}
#if 0
  else {
    if(resized) {
      init_XimageBuffer(width, height, depth);
    }
    window_snapshot(width, height, img->data);
    buffer_flip_vertical(width * BYTESPIXEL, height, img->data);
    XPutImage(dis, win, gc, img, 0, 0, 0, 0, width, height);
  }
#endif

#if 0
  printf("depth: %i bpl: %i bpp: %i xoffset %i\n", img->depth, img->bytes_per_line, img->bits_per_pixel, img->xoffset);
  printf("byte order: %i bitmap unit: %i bit order: %i bitmap_pad: %i \n", img->byte_order, img->bitmap_unit, img->bitmap_bit_order, img->bitmap_pad);
  printf("red_mask: %lu green_mask: %lu blue_mask: %lu \n", img->red_mask, img->green_mask, img->blue_mask);
  img->bits_per_pixel = 24;
  img->byte_order = MSBFirst;
  img->bitmap_bit_order = MSBFirst;
  img->bytes_per_line = window_attributes.width * BYTESPIXEL;
#endif
}

void xwindow_check_events(void)
{
  XEvent event;

  int cnt = XPending(dis);
  if (cnt > 0) {
    XNextEvent(dis, &event);
    /* keyboard events */
    switch(event.type)
    {
	    case KeyRelease:
      case KeyPress:
        xinput_check_keys(&event);
        break;

      //case KeyRelease:
      //  printf( "KeyRelease: %x\n", event.xkey.keycode );
      //  break;

      case ConfigureNotify:
        do_ConfigureNotify(&event.xconfigure);
        break;

      case ClientMessage:
        do_ClientMessage(&event.xclient);
        break;

      case ConfigureRequest:
        printf("configure request\n");
        break;

      case FocusOut:
        window_hide();
        break;

      case FocusIn:
        window_show();
        break;

      case VisibilityNotify:
        printf("visible state: %i\n", event.xvisibility.state);
        minimized = (event.xvisibility.state == 2) ? 1 : 0;
        break;

      case Expose:
        printf("x: %i, y: %i, width: %i, height %i\n",
          event.xexpose.x, event.xexpose.y, event.xexpose.width, event.xexpose.height);
        break;

      default:
        printf("unhandled event: %i\n", event.type);

    }
  }
}

