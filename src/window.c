/*
* window.c
*/

#include <stdio.h>
#include <assert.h>

#include "bcm_host.h"
#include "vc_vchi_dispmanx.h"
#include "gles3.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "print_info.h"
#include "gpu_texture.h"
#include "gpu_framebuffer.h"

#include "gldebug.h"

typedef  struct {
// OpenGL|ES objects
   EGLConfig config;
   EGLDisplay display;
   EGLSurface surface;
   EGLContext contextGLES2;
// EGL info
   int major;
   int minor;
   EGL_DISPMANX_WINDOW_T nativewindow;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;
   VC_RECT_T old_rect;
   int inFocus:1;
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

int window_screen_width(void)
{
  return window->nativewindow.width;
}

int window_screen_height(void)
{
  return window->nativewindow.height;
}

void window_update_old(void)
{
  window->old_rect.width = window->dst_rect.width;
  window->old_rect.height = window->dst_rect.height;
  window->old_rect.x = window->dst_rect.x;
  window->old_rect.y = window->dst_rect.y;
}

static void updateSrcSize(void)
{
  window->src_rect.width = (window->dst_rect.width) << 16;
  window->src_rect.height = (window->dst_rect.height) << 16;
  window->src_rect.x = ((window->nativewindow.width - window->dst_rect.width) / 2) << 16;
  window->src_rect.y = ((window->nativewindow.height - window->dst_rect.height) / 2) << 16;
}

void window_update(void)
{
  DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
  assert(update != 0);

  int result = vc_dispmanx_element_change_attributes(update,
                  window->nativewindow.element,
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

void window_hide(void)
{
  window_update_old();
  window->dst_rect.width = 1;
  window->dst_rect.height = 1;
  window->dst_rect.x = 0;
  window->dst_rect.y = 0;
  window->inFocus = 0;
  printf("hiding window \n");
  window_update();
}

void window_show(void)
{
  window->dst_rect.width = window->old_rect.width;
  window->dst_rect.height = window->old_rect.height;
  window->dst_rect.x = window->old_rect.x;
  window->dst_rect.y = window->old_rect.y;
  window->inFocus = 1;
  printf("showing window \n");
  window_update();
}

void window_size(const int width, const int height)
{
  window->dst_rect.width = width;
  window->dst_rect.height = height;
  window_update_old();
  updateSrcSize();
  window_update();
}

void window_pos(const int x, const int y)
{
  window->dst_rect.y = y;
  window->dst_rect.x = x;
  window_update_old();
  window_update();
}

void window_update_VSync(const int sync)
{
  const EGLBoolean result = eglSwapInterval(window->display, sync );
  assert(egl_chk(EGL_FALSE != result));
}

static void createSurface(void)
{
  DISPMANX_UPDATE_HANDLE_T dispman_update;

  int32_t success = graphics_get_display_size(0 /* LCD */, (uint32_t *)&window->nativewindow.width, (uint32_t *)&window->nativewindow.height);
  assert( success >= 0 );

  window->dst_rect.x = 0;
  window->dst_rect.y = 0;
  window->dst_rect.width = 1;
  window->dst_rect.height = 1;

  window->src_rect.x = 0;
  window->src_rect.y = 0;
  window->src_rect.width = 1;
  window->src_rect.height = 1;

  window->dispman_display = vc_dispmanx_display_open( 0 /* LCD */);

  dispman_update = vc_dispmanx_update_start( 0 );
  
  VC_DISPMANX_ALPHA_T alphadata = {
    DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS | DISPMANX_FLAGS_ALPHA_MIX | DISPMANX_FLAGS_ALPHA_PREMULT,
    0,
    0
  };
  
  window->nativewindow.element = vc_dispmanx_element_add( dispman_update, window->dispman_display,
    0/*layer*/, &window->dst_rect, 0/*src*/,
    &window->src_rect, DISPMANX_PROTECTION_NONE, &alphadata /*alpha*/, 0/*clamp*/, DISPMANX_ROTATE_90/*transform*/);

  vc_dispmanx_update_submit(dispman_update, 0, 0);

#if 0
  EGLint pixel_format = EGL_PIXEL_FORMAT_ARGB_8888_BRCM;

  pixel_format |= EGL_PIXEL_FORMAT_RENDER_GLES2_BRCM;
  pixel_format |= EGL_PIXEL_FORMAT_GLES2_TEXTURE_BRCM;

  EGLint pixmap[5];
        pixmap[0] = 0;
        pixmap[1] = 0;
        pixmap[2] = window->nativewindow.width;
        pixmap[3] = window->nativewindow.height;
        pixmap[4] = pixel_format;

  EGLint attrs[] = {
    EGL_VG_COLORSPACE, EGL_VG_COLORSPACE_sRGB,
    EGL_VG_ALPHA_FORMAT, pixel_format & EGL_PIXEL_FORMAT_ARGB_8888_PRE_BRCM ? EGL_VG_ALPHA_FORMAT_PRE : EGL_VG_ALPHA_FORMAT_NONPRE,
    EGL_NONE
  };

  eglCreateGlobalImageBRCM(window->nativewindow.width, window->nativewindow.height, pixmap[4], 0, window->nativewindow.width*4, pixmap);
  window->surface = eglCreatePixmapSurface(window->display, window->config, (EGLNativePixmapType)pixmap, attrs);
#else
  /*EGLint attribw[5];
        attribw[0] = EGL_HEIGHT;
        attribw[1] = window->nativewindow.height;
        attribw[2] = EGL_WIDTH;
        attribw[3] = window->nativewindow.width;
        attribw[4] = EGL_NONE;
  window->surface = eglCreatePbufferSurface(window->display, window->config, attribw);
  */
  window->surface = eglCreateWindowSurface( window->display, window->config, &window->nativewindow, NULL );
#endif
  assert(egl_chk(window->surface != EGL_NO_SURFACE));
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
    EGL_DEPTH_SIZE, 24,
    //EGL_STENCIL_SIZE, 8,
    //EGL_SAMPLE_BUFFERS, 1,
    //EGL_SAMPLES, 4,
    EGL_NONE
  };

  static EGLint context_attributes[] =
  {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };


  // get an EGL display connection
  window->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  assert(egl_chk(window->display!=EGL_NO_DISPLAY));

  // initialize the EGL display connection
  result = eglInitialize(window->display, &window->major, &window->minor);
  assert(egl_chk(EGL_FALSE != result));

  // get an appropriate EGL frame buffer configuration
  result = eglChooseConfig(window->display, attribute_list, &window->config, 1, &num_config);
  assert(egl_chk(EGL_FALSE != result));
  printf("number of configs available: %i\n", num_config);

  // bind the gles api to this thread - this is default so not required
  result = eglBindAPI(EGL_OPENGL_ES_API);
  assert(egl_chk(EGL_FALSE != result));

  // create an EGL rendering context

  window->contextGLES2 = eglCreateContext(window->display, window->config, EGL_NO_CONTEXT, context_attributes);
  assert(egl_chk(window->contextGLES2 != EGL_NO_CONTEXT));
}

#define TESTIT 0
#define WSCALE 1
static void window_setup_frameBufferRenderTexture(void)
{
  check_gl_error("starting setup frameBufferRenderTexture");
  // Build the texture that will serve as the color attachment for the framebuffer.
  const int tex = GPU_texture_create(window->nativewindow.width/WSCALE, window->nativewindow.height/WSCALE, GPU_RGB8, NULL);
  check_gl_error("make color texture buffer");

  // Build the texture that will serve as the depth attachment for the framebuffer.
  const int depth_tex = GPU_texture_create(window->nativewindow.width/WSCALE, window->nativewindow.height/WSCALE, GPU_DEPTH24, NULL);
  check_gl_error("make depth texture buffer");
  
  // Build the framebuffer.
  const int framebuffer = GPU_framebuffer_create();
  GPU_framebuffer_texture_attach(framebuffer, tex);
  GPU_framebuffer_texture_attach(framebuffer, depth_tex);
  GPU_framebuffer_bind(framebuffer);
  
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
    printf("Frame buffer incomplete: %s\n", get_FramebufferStatus_msg(status));

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
void window_init(void)
{
  EGLBoolean result;

  createContext();
  // create an EGL window surface based on current screen size
  createSurface();
  window_hide();
  // connect the context to the surface
  result = eglMakeCurrent(window->display, window->surface, window->surface, window->contextGLES2);
  assert(egl_chk(EGL_FALSE != result));

  // Set background color and clear buffers
  glClearColor(0.15f, 0.5f, 0.7f, 1.0f);

  // Enable back face culling.
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  //glLineWidth(1.0);
  //glEnable(GL_SAMPLE_COVERAGE);
  //glSampleCoverage(0.85, GL_FALSE);

  glPixelStorei(GL_PACK_ALIGNMENT, 4);

  glViewport(0, 0, (GLsizei)window_screen_width(), (GLsizei)window_screen_height());

  print_EGL_info();
  print_EGLSurface_info(window->surface);
  print_GL_Limits();

  for(int x=0; x<TESTIT; x++) window_setup_frameBufferRenderTexture();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void window_swap_buffers(void)
{
  assert(egl_chk(eglSwapBuffers(window->display, window->surface)));
  //assert(egl_chk(eglCopyBuffers(window->display, window->surface, &window->nativewindow)));
}

void window_release(void)
{
  eglMakeCurrent( window->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
  eglDestroySurface( window->display, window->surface );
  eglDestroyContext( window->display, window->contextGLES2 );
  eglTerminate( window->display );

  DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start(0);
  vc_dispmanx_element_remove( dispman_update, window->nativewindow.element );
  vc_dispmanx_update_submit_sync( dispman_update );

  vc_dispmanx_display_close(window->dispman_display);

}

void window_snapshot(const int width, const int height, void * buffer)
{
  const int x = (window->nativewindow.width - width) / 2;
  const int y = (window->nativewindow.height - height) / 2;
  glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

}

int window_inFocus(void)
{
  return window->inFocus;
}