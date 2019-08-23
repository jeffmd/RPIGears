// gldebug.c
#include <stdio.h>
#include "EGL/egl.h"
#include "GLES2/gl2.h"

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


int egl_chk(int result)
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

const char *get_FramebufferStatus_msg(GLenum error)
{
  switch (error) {
    CASE_CODE_RETURN_STR(GL_FRAMEBUFFER_COMPLETE)
    CASE_CODE_RETURN_STR(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
    CASE_CODE_RETURN_STR(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS)
    CASE_CODE_RETURN_STR(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
    CASE_CODE_RETURN_STR(GL_FRAMEBUFFER_UNSUPPORTED)
    default:
      return " ";
  }
}

static const char *get_gl_error_msg(GLenum error)
{
  switch (error) {
    CASE_CODE_RETURN_STR(GL_INVALID_ENUM)
    CASE_CODE_RETURN_STR(GL_INVALID_VALUE)
    CASE_CODE_RETURN_STR(GL_INVALID_OPERATION)
    CASE_CODE_RETURN_STR(GL_OUT_OF_MEMORY)
    CASE_CODE_RETURN_STR(GL_INVALID_FRAMEBUFFER_OPERATION)
    default:
      return " ";
  }
}

void check_gl_error(const char *msg)
{
  GLenum error = glGetError();
  while (error != GL_NO_ERROR) {
    printf("%s ERROR: %s\n", msg, get_gl_error_msg(error));
    error = glGetError();
  } 
}

