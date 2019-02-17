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
void glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
void glDisableVertexArrayAttrib(GLuint vaobj, GLuint index);
void glUseProgramMod(GLuint program);

void *glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);

void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primCount);
void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primCount);
void glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount, GLuint baseInstance);
void glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei instanceCount, GLuint baseInstance);
void glVertexAttribDivisor(GLuint index, GLuint divisor);


#define glVertexAttribPointer glVertexAttribPointerMod
#define glEnableVertexAttribArray glEnableVertexAttribArrayMod
#define glDisableVertexAttribArray glDisableVertexAttribArrayMod
#define glUseProgram glUseProgramMod
#define glUnmapBuffer glUnmapBufferOES
#define glMapBuffer glMapBufferOES
#define glBindBuffer glBindBufferMod

#endif
