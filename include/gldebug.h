// gldebug.h
#ifndef _GLDEBUG_H_
#define _GLDEBUG_H_

int egl_chk(int result);
const char *get_FramebufferStatus_msg(GLenum error);
void check_gl_error(const char *msg);

#endif
