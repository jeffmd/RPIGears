/*
* matrix_math.h
*/

#ifndef _matrix_math_h_
  #define _matrix_math_h_
  
  #include "GLES/gl.h"

  
  extern void m4x4_copy(GLfloat *md, const GLfloat *ms);
  extern void m4x4_multiply(GLfloat *m, const GLfloat *n);
  extern void m4x4_rotate(GLfloat *m, GLfloat angle, const GLfloat x, const GLfloat y, const GLfloat z);
  extern void m4x4_translate(GLfloat *m, const GLfloat x, const GLfloat y, const GLfloat z);
  extern void m4x4_identity(GLfloat *m);
  extern void m4x4_transpose(GLfloat *m);
  extern void m4x4_invert(GLfloat *m);
  extern void m4x4_perspective(GLfloat *m, const GLfloat fovy, const GLfloat aspect, const GLfloat zNear, const GLfloat zFar);
  extern void m4xv3(float r[3], const float mat[16], const float vec[3]);
#endif
