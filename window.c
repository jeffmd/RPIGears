/*
* window.c
*/

#include <stdio.h>
#include <assert.h>

#include "bcm_host.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
//#include "EGL/eglext_brcm.h"


//  Element Attributes changes flag mask
#define ELEMENT_CHANGE_LAYER          (1<<0)
#define ELEMENT_CHANGE_OPACITY        (1<<1)
#define ELEMENT_CHANGE_DEST_RECT      (1<<2)
#define ELEMENT_CHANGE_SRC_RECT       (1<<3)
#define ELEMENT_CHANGE_MASK_RESOURCE  (1<<4)
#define ELEMENT_CHANGE_TRANSFORM      (1<<5)

#define CASE_CODE_RETURN_STR(code) case code: return #code;

static const char *get_egl_error_enum_string(EGLenum error)
{
	switch (error) {
		CASE_CODE_RETURN_STR(EGL_SUCCESS)
		CASE_CODE_RETURN_STR(EGL_NOT_INITIALIZED)
		CASE_CODE_RETURN_STR(EGL_BAD_ACCESS)
		CASE_CODE_RETURN_STR(EGL_BAD_ALLOC)
		CASE_CODE_RETURN_STR(EGL_BAD_ATTRIBUTE)
		CASE_CODE_RETURN_STR(EGL_BAD_CONTEXT)
		CASE_CODE_RETURN_STR(EGL_BAD_CONFIG)
		CASE_CODE_RETURN_STR(EGL_BAD_CURRENT_SURFACE)
		CASE_CODE_RETURN_STR(EGL_BAD_DISPLAY)
		CASE_CODE_RETURN_STR(EGL_BAD_SURFACE)
		CASE_CODE_RETURN_STR(EGL_BAD_MATCH)
		CASE_CODE_RETURN_STR(EGL_BAD_PARAMETER)
		CASE_CODE_RETURN_STR(EGL_BAD_NATIVE_PIXMAP)
		CASE_CODE_RETURN_STR(EGL_BAD_NATIVE_WINDOW)
		CASE_CODE_RETURN_STR(EGL_CONTEXT_LOST)
		default:
			return NULL;
	}
}

static const char *get_egl_error_message_string(EGLenum error)
{
	switch (error) {
		case EGL_SUCCESS:
			return "The last function succeeded without error.";

		case EGL_NOT_INITIALIZED:
			return ("EGL is not initialized, or could not be initialized, "
			        "for the specified EGL display connection.");

		case EGL_BAD_ACCESS:
			return ("EGL cannot access a requested resource "
			        "(for example a context is bound in another thread).");

		case EGL_BAD_ALLOC:
			return "EGL failed to allocate resources for the requested operation.";

		case EGL_BAD_ATTRIBUTE:
			return "An unrecognized attribute or attribute value was passed in the attribute list.";

		case EGL_BAD_CONTEXT:
			return "An EGLContext argument does not name a valid EGL rendering context.";

		case EGL_BAD_CONFIG:
			return "An EGLConfig argument does not name a valid EGL frame buffer configuration.";

		case EGL_BAD_CURRENT_SURFACE:
			return ("The current surface of the calling thread is a window, "
			        "pixel buffer or pixmap that is no longer valid.");

		case EGL_BAD_DISPLAY:
			return "An EGLDisplay argument does not name a valid EGL display connection.";

		case EGL_BAD_SURFACE:
			return ("An EGLSurface argument does not name a valid surface "
			        "(window, pixel buffer or pixmap) configured for GL rendering.");

		case EGL_BAD_MATCH:
			return ("Arguments are inconsistent "
			        "(for example, a valid context requires buffers not supplied by a valid surface).");

		case EGL_BAD_PARAMETER:
			return "One or more argument values are invalid.";

		case EGL_BAD_NATIVE_PIXMAP:
			return "A NativePixmapType argument does not refer to a valid native pixmap.";

		case EGL_BAD_NATIVE_WINDOW:
			return "A NativeWindowType argument does not refer to a valid native window.";

		case EGL_CONTEXT_LOST:
			return ("A power management event has occurred. "
			        "The application must destroy all contexts and reinitialise OpenGL ES state "
			        "and objects to continue rendering.");

		default:
			return NULL;
	}
}


static int egl_chk(int result)
{
	if (!result) {
		EGLenum error = eglGetError();

		const char *code = get_egl_error_enum_string(error);
		const char *msg  = get_egl_error_message_string(error);

		printf(
		        "EGL Error (0x%04X): %s: %s\n",
		        error,
		        code ? code : "<Unknown>",
		        msg  ? msg  : "<Unknown>");
	}

	return result;
}


typedef  struct {
   // window data
   //uint32_t screen_width;
   //uint32_t screen_height;
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
   float pos_x;
   float pos_y;
   float width;
   float height;
   int update; // if > 0 then window needs updating in dispmanx
   int inFocus;
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
  return window->nativewindow.width;
}

uint32_t window_screen_height(void)
{
  return window->nativewindow.height;
}

void window_update_pos(void)
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
  window_update_pos();
  window->update = 1;
}

void window_center(void)
{
  window->dst_rect.x = window->nativewindow.width/4;
  window->dst_rect.y = window->nativewindow.height/4;
  window_update_pos();
  window->update = 1;
}

void window_show(void)
{
  window->dst_rect.x = window->pos_x;
  window->dst_rect.y = window->pos_y;
  window->update = 1;
  window->inFocus = 1;
  printf("showing window \n");
}

void window_hide(void)
{
  window_update_pos();
  window->dst_rect.x = window->nativewindow.width;
  window->dst_rect.y = window->nativewindow.height;
  window->update = 1;
  window->inFocus = 0;
  printf("hiding window \n");
}

void window_zoom(const float val)
{
  window->width += val;
  window->height += val;
  window->dst_rect.width = (int)window->width;
  window->dst_rect.height = (int)window->height;
  window->update = 1;
}

void window_size(const int width, const int height)
{
  window->width = width;
  window->height = height;
  window->dst_rect.width = width;
  window->dst_rect.height = height;
  window->update = 1;
}

static void updateSrcSize(void)
{
	window->src_rect.width = (window->dst_rect.width) << 16;
	window->src_rect.height = (window->dst_rect.height) << 16;
    window->src_rect.x = ((window->nativewindow.width - window->dst_rect.width) / 2) << 16;
    window->src_rect.y = ((window->nativewindow.height - window->dst_rect.height) / 2) << 16;
}


static void check_window_offsets(void)
{
  if (window->dst_rect.x <= -window->dst_rect.width) {
    window->dst_rect.x = -window->dst_rect.width + 1;
    window->pos_x = (float)window->dst_rect.x;
  }
  else
    if (window->dst_rect.x > (int)window->nativewindow.width) {
      window->dst_rect.x = (int)window->nativewindow.width;
      window->pos_x = (float)window->dst_rect.x; 
    }
       
  if (window->dst_rect.y <= -window->dst_rect.height) {
    window->dst_rect.y = -window->dst_rect.height + 1;
    window->pos_y = (float)window->dst_rect.y; 
  }  
  else
    if (window->dst_rect.y > (int)window->nativewindow.height) {
       window->dst_rect.y = (int)window->nativewindow.height;
       window->pos_y = (float)window->dst_rect.y;
    }
}

void window_update(void)
{
  if (window->update) {
    check_window_offsets();
    
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
    assert(update != 0);
    updateSrcSize();
    
    int result = vc_dispmanx_element_change_attributes(update,
                                            window->nativewindow.element,
                                            ELEMENT_CHANGE_OPACITY,
                                            0,
                                            128,
                                            &(window->dst_rect),
                                            &(window->src_rect),
                                            0,
                                            DISPMANX_ROTATE_90);
      assert(result == 0);
  
      result = vc_dispmanx_update_submit(update, 0, 0);
      assert(result == 0);
      window->update = 0;
      //window_init_pos();
    }
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
  
  window->nativewindow.element = vc_dispmanx_element_add( dispman_update, window->dispman_display,
    0/*layer*/, &window->dst_rect, 0/*src*/,
    &window->src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);
  
  vc_dispmanx_update_submit_sync( dispman_update );
  
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
    EGL_DEPTH_SIZE, 8,
    EGL_SAMPLE_BUFFERS, 1,
    EGL_SAMPLES, 4,
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

static void window_print_GL_Limits(void)
{
  GLint num[4];
  
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, num);
  printf("MAX_VERTEX_ATTRIBS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_VARYING_VECTORS, num);
  printf("MAX_VARYING_VECTORS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, num);
  printf("MAX_FRAGMENT_UNIFORM_VECTORS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, num);
  printf("MAX_VERTEX_UNIFORM_VECTORS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, num);
  printf("MAX_VERTEX_TEXTURE_IMAGE_UNITS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, num);
  printf("MAX_TEXTURE_IMAGE_UNITS: %i\n", num[0]);

  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, num);
  printf("MAX_COMBINED_TEXTURE_IMAGE_UNITS: %i\n", num[0]);

  glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, num);
  printf("NUM_COMPRESSED_TEXTURE_FORMATS: %i\n", num[0]);

  glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, num);
  printf("NUM_SHADER_BINARY_FORMATS: %i\n", num[0]);

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
  
  window->inFocus = 1;
  createContext();
  // create an EGL window surface based on current screen size
  createSurface();
  
  // connect the context to the surface
  result = eglMakeCurrent(window->display, window->surface, window->surface, window->contextGLES2);
  assert(egl_chk(EGL_FALSE != result));
  
  // Set background color and clear buffers
  glClearColor(0.25f, 0.45f, 0.55f, 1.0f);
  
  // Enable back face culling.
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_SAMPLE_COVERAGE);
  //glSampleCoverage(0.85, GL_FALSE);
  
  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  
  window_print_GL_Limits();

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
