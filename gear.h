/*
* gear.h
*/
#ifndef _GEAR_H_
  #define _GEAR_H_

  #include "GLES/gl.h"
  
  extern int gear( const GLfloat inner_radius, const GLfloat outer_radius,
                       const GLfloat width, const GLint teeth,
                       const GLfloat tooth_depth, const GLfloat color[]);
  
  extern void set_gear_va_ptrs(int gearid);
  extern void make_gear_vbo(int gearid);
  extern void free_gear(int gearid);
  extern void gear_use_vbo(const int gearid);
  extern void gear_drawGLES1(const int gearid, const int useVBO,const GLenum drawMode);
  extern void gear_drawGLES2(const int gearid, const int useVBO,const GLenum drawMode, const GLuint MaterialColor_location);

#endif  
  
