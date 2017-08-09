/*
* matrix_math.h
*/

#ifndef _matrix_math_h_
  #define _matrix_math_h_
  
  extern void m4x4_copy(GLfloat *md, const GLfloat *ms);
  extern void m4x4_multiply(GLfloat *m, const GLfloat *n);
  extern void m4x4_rotate(GLfloat *m, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  extern void m4x4_translate(GLfloat *m, GLfloat x, GLfloat y, GLfloat z);
  extern void m4x4_identity(GLfloat *m);
  extern void m4x4_transpose(GLfloat *m);
  extern void m4x4_invert(GLfloat *m);
  extern void m4x4_perspective(GLfloat *m, GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);
#endif
