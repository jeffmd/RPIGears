/*
* gear.h
*/
#ifndef _GEAR_H_
  #define _GEAR_H_

  #include "GLES/gl.h"
  
  int gear( const GLfloat inner_radius, const GLfloat outer_radius,
                       const GLfloat width, const GLint teeth,
                       const GLfloat tooth_depth, const GLfloat color[],
                       const int useVBO);
  
  void set_gear_va_ptrs(int gearid);
  void make_gear_vbo(int gearid);
  void free_gear(int gearid);
  void gear_use_vbo(const int gearid);
  void gear_drawGLES1(const int gearid, const int useVBO, const GLenum drawMode);
  void gear_drawGLES2(const int gearid, const int useVBO, const GLenum drawMode, const GLuint MaterialColor_location);
  void gear_setVAO_GLES2(const int gearid, const int useVBO);

#endif  
  
