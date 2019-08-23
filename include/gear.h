/*
* gear.h
*/
#ifndef _GEAR_H_
  #define _GEAR_H_

  int gear( const GLfloat inner_radius, const GLfloat outer_radius,
                       const GLfloat width, const GLint teeth,
                       const GLfloat tooth_depth,
                       const GLfloat color[]);
  
  void gear_delete(const int id);
  void gear_draw(const int id, const GLenum drawMode, const GLuint instances);
  void gear_use_BO(const int id);
  void gear_no_BO(const int id);

#endif  
  
