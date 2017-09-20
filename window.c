/*
* window.c
*/

#include <assert.h>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "EGL/egl.h"

//  Element Attributes changes flag mask
#define ELEMENT_CHANGE_LAYER          (1<<0)
#define ELEMENT_CHANGE_OPACITY        (1<<1)
#define ELEMENT_CHANGE_DEST_RECT      (1<<2)
#define ELEMENT_CHANGE_SRC_RECT       (1<<3)
#define ELEMENT_CHANGE_MASK_RESOURCE  (1<<4)
#define ELEMENT_CHANGE_TRANSFORM      (1<<5)

typedef  struct {
   // window data
   uint32_t screen_width;
   uint32_t screen_height;
// OpenGL|ES objects
   EGLConfig config;
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


static WINDOW_T _window;
static WINDOW_T * const window = &_window;

int window_major(void)
{
  return window->major;
}

int window_minor(void)
{
  return window->minor;
}

EGLDisplay window_display(void)
{
  return window->display;
}

uint32_t window_screen_width(void)
{
  return window->screen_width;
}

uint32_t window_screen_height(void)
{
  return window->screen_height;
}

void window_init_pos(void)
{
  window->pos_x = (float)window->dst_rect.x;
  window->pos_y = (float)window->dst_rect.y;
}

void window_init_size(void)
{
  window->width = (float)window->dst_rect.width;
  window->height = (float)window->dst_rect.height;
}


void window_move_x(const float val)
{
  window->pos_x += val;
  window->dst_rect.x = (int)window->pos_x;
  window->update = 1;
}


void window_move_y(const float val)
{
  window->pos_y += val;
  window->dst_rect.y = (int)window->pos_y;
  window->update = 1;
}

void window_pos(const int x, const int y)
{
  window->dst_rect.y = y;
  window->dst_rect.x = x;
  window->update = 1;
}

void window_center(void)
{
  window->dst_rect.x = window->screen_width/4;
  window->dst_rect.y = window->screen_height/4;
  window_init_pos();
  window->update = 1;
}

void window_show(void)
{
  window->dst_rect.x = window->pos_x;
  window->dst_rect.y = window->pos_y;
  window->update = 1;
}

void window_hide(void)
{
  window_init_pos();
  window->dst_rect.x = window->screen_width;
  window->dst_rect.y = window->screen_height;
  window->update = 1;
}

void window_zoom(const float val)
{
  window->width += val;
  window->height += val;
  window->dst_rect.width = (int)window->width;
  window->dst_rect.height = (int)window->height;
  window->update = 1;
}

static void check_window_offsets(void)
{
  if (window->dst_rect.x <= -window->dst_rect.width) {
    window->dst_rect.x = -window->dst_rect.width + 1;
    window->pos_x = (float)window->dst_rect.x;
  }
  else
    if (window->dst_rect.x > (int)window->screen_width) {
      window->dst_rect.x = (int)window->screen_width;
      window->pos_x = (float)window->dst_rect.x; 
    }
       
  if (window->dst_rect.y <= -window->dst_rect.height) {
    window->dst_rect.y = -window->dst_rect.height + 1;
    window->pos_y = (float)window->dst_rect.y; 
  }  
  else
    if (window->dst_rect.y > (int)window->screen_height) {
       window->dst_rect.y = (int)window->screen_height;
       window->pos_y = (float)window->dst_rect.y;
    }
}

void Window_update(void)
{
  if (window->update) {
    check_window_offsets();
    
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);
  
    int result = vc_dispmanx_element_change_attributes(update,
                                            window->dispman_element,
                                            ELEMENT_CHANGE_DEST_RECT,
                                            0,
                                            255,
                                            &(window->dst_rect),
                                            &(window->src_rect),
                                            0,
                                            DISPMANX_ROTATE_90);
      assert(result == 0);
  
      result = vc_dispmanx_update_submit(update, 0, 0);
      assert(result == 0);
      window->update = 0;
    }
}

void window_update_VSync(const int sync)
{
  const EGLBoolean result = eglSwapInterval(window->display, sync );
  assert(EGL_FALSE != result);
}

static void createSurface(void)
{
  DISPMANX_UPDATE_HANDLE_T dispman_update;

  int32_t success = graphics_get_display_size(0 /* LCD */, &window->screen_width, &window->screen_height);
  assert( success >= 0 );
  
  window->dst_rect.x = window->screen_width/4;
  window->dst_rect.y = window->screen_height/4;
  window->dst_rect.width = window->screen_width/2;
  window->dst_rect.height = window->screen_height/2;
  
  window->src_rect.x = 0;
  window->src_rect.y = 0;
  window->src_rect.width = (window->screen_width/2) << 16;
  window->src_rect.height = (window->screen_height/2) << 16;
  
  window->dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
  
  dispman_update = vc_dispmanx_update_start( 0 );
  
  window->dispman_element = vc_dispmanx_element_add( dispman_update, window->dispman_display,
    0/*layer*/, &window->dst_rect, 0/*src*/,
    &window->src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);
  
  vc_dispmanx_update_submit_sync( dispman_update );
  
  window->nativewindow.element = window->dispman_element;
  window->nativewindow.width = window->screen_width;
  window->nativewindow.height = window->screen_height;

  window->surface = eglCreateWindowSurface( window->display, window->config, &window->nativewindow, NULL );
  assert(window->surface != EGL_NO_SURFACE);
}

static void createContext(void)
{
  EGLBoolean result;
  EGLint num_config;
  
  static const EGLint attribute_list[] =
  {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 16,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_NONE
  };
  
  static EGLint context_attributes[] = 
  {
    EGL_CONTEXT_CLIENT_VERSION, 1,
    EGL_NONE
  };
  
  
  // get an EGL display connection
  window->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  assert(window->display!=EGL_NO_DISPLAY);
  
  // initialize the EGL display connection
  result = eglInitialize(window->display, &window->major, &window->minor);
  assert(EGL_FALSE != result);
  
  // get an appropriate EGL frame buffer configuration
  result = eglChooseConfig(window->display, attribute_list, &window->config, 1, &num_config);
  assert(EGL_FALSE != result);
  
  // bind the gles api to this thread - this is default so not required
  result = eglBindAPI(EGL_OPENGL_ES_API);
  assert(EGL_FALSE != result);
  
  // create an EGL rendering context
  // select es 1.x or 2.x based on user option
  context_attributes[1] = 1;
  window->contextGLES1 = eglCreateContext(window->display, window->config, EGL_NO_CONTEXT, context_attributes);
  assert(window->contextGLES1 != EGL_NO_CONTEXT);
  
  context_attributes[1] = 2;
  window->contextGLES2 = eglCreateContext(window->display, window->config, EGL_NO_CONTEXT, context_attributes);
  assert(window->contextGLES2 != EGL_NO_CONTEXT);
}

/***********************************************************
 * Name: init_window
 *
 *
 * Description: Sets the display, OpenGL|ES context and screen stuff
 *
 * Returns: void
 *
 ***********************************************************/
void window_init(const int useGLES2)
{
  EGLBoolean result;
  
  createContext();
  // create an EGL window surface based on current screen size
  createSurface();
  
  // connect the context to the surface
  result = eglMakeCurrent(window->display, window->surface, window->surface, useGLES2 ? window->contextGLES2 : window->contextGLES1);
  assert(EGL_FALSE != result);
  
  // Set background color and clear buffers
  glClearColor(0.25f, 0.45f, 0.55f, 0.50f);
  
  // Enable back face culling.
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);

}

void window_swap_buffers(void)
{
  eglSwapBuffers(window->display, window->surface);
}

void window_release(void)
{
  eglMakeCurrent( window->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
  eglDestroySurface( window->display, window->surface );
  eglDestroyContext( window->display, window->contextGLES1 );
  eglDestroyContext( window->display, window->contextGLES2 );
  eglTerminate( window->display );
}
