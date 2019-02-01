/*
* matrix_math.h
*/

#ifndef _matrix_math_h_
  #define _matrix_math_h_
  
  void m4x4_copy(GLfloat *md, const GLfloat *ms);
  void m4x4_multiply(GLfloat *m, const GLfloat *n);
  void m4x4_rotate(GLfloat *m, GLfloat angle, const GLfloat x, const GLfloat y, const GLfloat z);
  void m4x4_translate(GLfloat *m, const GLfloat x, const GLfloat y, const GLfloat z);
  void m4x4_identity(GLfloat *m);
  void m4x4_transpose(GLfloat *m);
  void m4x4_invert(GLfloat *m);
  void m4x4_perspective(GLfloat *m, const GLfloat fovy, const GLfloat aspect, const GLfloat zNear, const GLfloat zFar);
  void m4xv3(float r[3], const float mat[16], const float vec[3]);
#endif
