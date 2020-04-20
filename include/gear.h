/*
* gear.h
*/
#ifndef _GEAR_H_
  #define _GEAR_H_

  short gear( const GLfloat inner_radius, const GLfloat outer_radius,
                       const GLfloat width, const GLint teeth,
                       const GLfloat tooth_depth,
                       const GLfloat color[]);
  
  void gear_delete(const short id);
  void gear_draw(const short id, const GLenum drawMode, const GLuint instances);
  void gear_use_BO(const short id);
  void gear_no_BO(const short id);

#endif  
  
