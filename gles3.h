/* gles3.h */
/* implementation of vertex array object and other opengl ES 3.0 functions for gles2 on Raspberry Pi */
#ifndef _GLES3_H_
  #define _GLES3_H_

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

void gles3_init(void);
void glVertexAttribPointerMod(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr);
void glEnableVertexAttribArrayMod(GLuint index);
void glDisableVertexAttribArrayMod(GLuint index);
void glBindBufferMod(GLenum target, GLuint buffer);
void glBindVertexArray(GLuint array);
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
void glGenVertexArrays(GLsizei n, GLuint *arrays);
void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
void glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
void glDisableVertexArrayAttrib(GLuint vaobj, GLuint index);

void *glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);


#define glVertexAttribPointer glVertexAttribPointerMod
#define glEnableVertexAttribArray glEnableVertexAttribArrayMod
#define glDisableVertexAttribArray glDisableVertexAttribArrayMod
#define glUnmapBuffer glUnmapBufferOES
#define glMapBuffer glMapBufferOES
#define glBindBuffer glBindBufferMod

#endif
