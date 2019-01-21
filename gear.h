/*
* gear.h
*/
#ifndef _GEAR_H_
  #define _GEAR_H_

  #include "GLES2/gl2.h"
  
  int gear( const GLfloat inner_radius, const GLfloat outer_radius,
                       const GLfloat width, const GLint teeth,
                       const GLfloat tooth_depth, const GLfloat color[],
                       const int useVBO);
  
  void set_gear_va_ptrs(int gearid);
  void make_gear_vbo(int gearid);
  void free_gear(int gearid);
  void gear_use_vbo(const int gearid);
  void gear_draw(const int gearid, const GLenum drawMode, const GLuint MaterialColor_location);
  void gear_setVAO(const int gearid, const int useVBO);

#endif  
  
