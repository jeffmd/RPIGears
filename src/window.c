/*
* window.c
*/

#include <stdio.h>
#include <assert.h>

#include "bcm_host.h"
#include "interface/vmcs_host/vc_vchi_dispmanx.h"
#include "gles3.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "print_info.h"

#include "gldebug.h"

typedef  struct {
// OpenGL|ES objects
   EGLConfig config;
   EGLDisplay egl_display;
   EGLSurface surface;
   EGLContext contextGLES2;
// EGL info
   int major;
   int minor;
   EGL_DISPMANX_WINDOW_T native_window;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;
   VC_RECT_T old_rect;
   int offsetx;
   int offsety;
   int inFocus:1;
} WINDOW_T;


static WINDOW_T _window;
static WINDOW_T * const window = &_window;

int Window_major(void)
{
  return window->major;
}

int Window_minor(void)
{
  return window->minor;
}

int Window_screen_width(void)
{
  return window->native_window.width;
}

int Window_screen_height(void)
{
  return window->native_window.height;
}

static void window_update_old(void)
{
  window->old_rect.width = window->dst_rect.width;
  window->old_rect.height = window->dst_rect.height;
  window->old_rect.x = window->dst_rect.x;
  window->old_rect.y = window->dst_rect.y;
}

static void update_src_size(void)
{
  window->src_rect.width = (window->dst_rect.width) << 16;
  window->src_rect.height = (window->dst_rect.height) << 16;
  window->offsetx = (window->native_window.width - window->dst_rect.width) / 2;
  window->src_rect.x = window->offsetx << 16;
  window->offsety = (window->native_window.height - window->dst_rect.height) / 2;
  window->src_rect.y = window->offsety << 16;
}

static void window_update(void)
{
  DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
  assert(update != 0);

  int result = vc_dispmanx_element_change_attributes(update,
                  window->native_window.element,
                  0/*ELEMENT_CHANGE_OPACITY | ELEMENT_CHANGE_TRANSFORM*/,
                  0,
                  0,
                  &window->dst_rect,
                  &window->src_rect,
                  0,
                  0);
  
  assert(result == 0);

  result = vc_dispmanx_update_submit(update, 0, 0);
  assert(result == 0);
}

static void window_size_reset(void)
{
  window->dst_rect.width = 1;
  window->dst_rect.height = 1;
  window->dst_rect.x = 0;
  window->dst_rect.y = 0;
}

void Window_hide(void)
{
  window_update_old();
  window_size_reset();
  window->inFocus = 0;
  //printf("hiding window \n");
  window_update();
}

void Window_show(void)
{
  window->dst_rect.width = window->old_rect.width;
  window->dst_rect.height = window->old_rect.height;
  window->dst_rect.x = window->old_rect.x;
  window->dst_rect.y = window->old_rect.y;
  window->inFocus = 1;
  //printf("showing window \n");
  window_update();
}

void Window_size(const int width, const int height)
{
  window->dst_rect.width = width;
  window->dst_rect.height = height;
  window_update_old();
  update_src_size();
  window_update();
}

void Window_pos(const int x, const int y)
{
  window->dst_rect.y = y;
  window->dst_rect.x = x;
  window_update_old();
  window_update();
}

void Window_update_VSync(const int sync)
{
  const EGLBoolean result = eglSwapInterval(window->egl_display, sync );
  assert(egl_chk(EGL_FALSE != result));
}

static void create_native_window(void)
{
  DISPMANX_UPDATE_HANDLE_T dispman_update;

  int32_t success = graphics_get_display_size(0 /* LCD */, (uint32_t *)&window->native_window.width, (uint32_t *)&window->native_window.height);
  assert( success >= 0 );

  window_size_reset();
  update_src_size();

  window->dispman_display = vc_dispmanx_display_open( 0 /* LCD */);

  VC_DISPMANX_ALPHA_T alphadata = {
    DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS | DISPMANX_FLAGS_ALPHA_MIX | DISPMANX_FLAGS_ALPHA_PREMULT,
    0,
    0
  };
  
  printf("creating native window: dispmanx element\n");
  dispman_update = vc_dispmanx_update_start( 0 );
 
  window->native_window.element = vc_dispmanx_element_add( dispman_update, window->dispman_display,
    0/*layer*/, &window->dst_rect, 0/*src*/,
    &window->src_rect, DISPMANX_PROTECTION_NONE, &alphadata /*alpha*/, 0/*clamp*/, DISPMANX_ROTATE_90/*transform*/);
  vc_dispmanx_update_submit(dispman_update, 0, 0);

  Window_hide();
}

static NativeWindowType get_native_window(void)
{

  if (!window->native_window.element) {
    create_native_window();
  }

  return &window->native_window;
}

static void create_egl_display(void)
{
  // get an EGL display connection
  printf("creating egl display\n");
  window->egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  assert(egl_chk(window->egl_display!=EGL_NO_DISPLAY));

  // initialize the EGL display connection
  EGLBoolean result = eglInitialize(window->egl_display, &window->major, &window->minor);
  assert(egl_chk(EGL_FALSE != result));
}

static EGLDisplay get_egl_display(void)
{
  if (!window->egl_display) {
    create_egl_display();
  }

  return window->egl_display;
}

static void create_config(void)
{
  EGLBoolean result;
  EGLint num_config;

  static const EGLint attribute_list[] =
  {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    //EGL_STENCIL_SIZE, 8,
    //EGL_SAMPLE_BUFFERS, 1,
    //EGL_SAMPLES, 4,
    EGL_NONE
  };

  // get an appropriate EGL frame buffer configuration
  printf("creating EGL configuration\n");
  result = eglChooseConfig(get_egl_display(), attribute_list, &window->config, 1, &num_config);
  assert(egl_chk(EGL_FALSE != result));
  printf("number of configs available: %i\n", num_config);

  // bind the gles api to this thread - this is default so not required
  result = eglBindAPI(EGL_OPENGL_ES_API);
  assert(egl_chk(EGL_FALSE != result));
}

static EGLConfig get_config(void)
{
  if (!window->config) {
    create_config();
  }

  return window->config;
}

static void create_context(void)
{
  static EGLint context_attributes[] =
  {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };

  // create an EGL rendering context
  printf("creating GLES 2 context\n");
  window->contextGLES2 = eglCreateContext(get_egl_display(), get_config(), EGL_NO_CONTEXT, context_attributes);
  assert(egl_chk(window->contextGLES2 != EGL_NO_CONTEXT));
}

static EGLContext get_context(void)
{
  if (!window->contextGLES2) {
    create_context();
  }

  return window->contextGLES2;
}

static void create_surface(void)
{

#if 0
  EGLint pixel_format = EGL_PIXEL_FORMAT_ARGB_8888_BRCM;

  pixel_format |= EGL_PIXEL_FORMAT_RENDER_GLES2_BRCM;
  pixel_format |= EGL_PIXEL_FORMAT_GLES2_TEXTURE_BRCM;

  EGLint pixmap[5];
        pixmap[0] = 0;
        pixmap[1] = 0;
        pixmap[2] = window->native_window.width;
        pixmap[3] = window->native_window.height;
        pixmap[4] = pixel_format;

  EGLint attrs[] = {
    EGL_VG_COLORSPACE, EGL_VG_COLORSPACE_sRGB,
    EGL_VG_ALPHA_FORMAT, pixel_format & EGL_PIXEL_FORMAT_ARGB_8888_PRE_BRCM ? EGL_VG_ALPHA_FORMAT_PRE : EGL_VG_ALPHA_FORMAT_NONPRE,
    EGL_NONE
  };

  eglCreateGlobalImageBRCM(window->native_window.width, window->native_window.height, pixmap[4], 0, window->native_window.width*4, pixmap);
  window->surface = eglCreatePixmapSurface(window->display, window->config, (EGLNativePixmapType)pixmap, attrs);
#else
  /*EGLint attribw[5];
        attribw[0] = EGL_HEIGHT;
        attribw[1] = window->native_window.height;
        attribw[2] = EGL_WIDTH;
        attribw[3] = window->native_window.width;
        attribw[4] = EGL_NONE;
  window->surface = eglCreatePbufferSurface(window->display, window->config, attribw);
  */
  printf("creating EGL window surface\n");
  window->surface = eglCreateWindowSurface( get_egl_display(), get_config(), get_native_window(), NULL );
#endif
  assert(egl_chk(window->surface != EGL_NO_SURFACE));
}

static EGLSurface get_surface(void)
{
  if (!window->surface) {
    create_surface();
  }

  return window->surface;
}

void Window_init(void)
{
  // connect the context to the surface
  EGLBoolean result = eglMakeCurrent(get_egl_display(), get_surface(), get_surface(), get_context());
  assert(egl_chk(EGL_FALSE != result));

  // Set background color and clear buffers
  glClearColor(0.15f, 0.5f, 0.7f, 1.0f);

  // Enable back face culling.
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  //glLineWidth(1.0);
  //glEnable(GL_SAMPLE_COVERAGE);
  //glSampleCoverage(0.85, GL_FALSE);

  glPixelStorei(GL_PACK_ALIGNMENT, 4);

  Print_EGL_info();
  Print_EGLSurface_info(window->surface);
  Print_GL_Limits();
}

void Window_swap_buffers(void)
{
  assert(egl_chk(eglSwapBuffers(window->egl_display, window->surface)));
  //assert(egl_chk(eglCopyBuffers(window->display, window->surface, &window->native_window)));
}

void Window_viewport_reset(void)
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, window->native_window.width, window->native_window.height);
  glDisable(GL_SCISSOR_TEST);
}

void Window_ui_viewport(short pos[2], short size[2], short vis_pos[4])
{
  short y = window->old_rect.height - pos[1] - size[1];
  const int ox = pos[0] + (size[0] - window->old_rect.width ) / 2 ;
  const int oy = y + (size[1] - window->old_rect.height) / 2;
  glViewport(ox, oy, window->native_window.width, window->native_window.height);
  y = window->old_rect.height - vis_pos[3];
  glScissor(window->offsetx + vis_pos[0], window->offsety + y, vis_pos[2] - vis_pos[0], vis_pos[3] - vis_pos[1]);
}

void Window_release(void)
{
  eglMakeCurrent( window->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
  eglDestroySurface( window->egl_display, window->surface );
  window->surface = 0;
  eglDestroyContext( window->egl_display, window->contextGLES2 );
  window->contextGLES2 = 0;
  eglTerminate( window->egl_display );
  window->egl_display = 0;

  DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start(0);
  vc_dispmanx_element_remove( dispman_update, window->native_window.element );
  window->native_window.element = 0;
  vc_dispmanx_update_submit_sync( dispman_update );

  vc_dispmanx_display_close(window->dispman_display);
  window->dispman_display = 0;

}

void Window_snapshot(const int width, const int height, void * buffer)
{
  const int x = (window->native_window.width - width) / 2;
  const int y = (window->native_window.height - height) / 2;
  glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

}

int Window_inFocus(void)
{
  return window->inFocus;
}
