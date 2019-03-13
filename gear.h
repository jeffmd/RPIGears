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
  
  void free_gear(int gearid);
  void gear_draw(const int gearid, const GLenum drawMode, const GLuint MaterialColor_location, const GLuint instances);
#endif  
  
