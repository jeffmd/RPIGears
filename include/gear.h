/*
* gear.h
*/
#ifndef _GEAR_H_
  #define _GEAR_H_

  short Gear_create( const GLfloat inner_radius, const GLfloat outer_radius,
                       const GLfloat width, const GLint teeth,
                       const GLfloat tooth_depth,
                       const GLfloat color[]);
  
  void Gear_delete(const short id);
  void Gear_draw(const short id, const GLenum drawMode, const GLuint instances);
  void Gear_use_BO(const short id);
  void Gear_no_BO(const short id);

#endif  
  
