/*
* gear.h
*/
#ifndef _GEAR_H_
  #define _GEAR_H_

  ID_t Gear_create( const GLfloat inner_radius, const GLfloat outer_radius,
                       const GLfloat width, const GLint teeth,
                       const GLfloat tooth_depth,
                       const GLfloat color[4]);
  
  void Gear_delete(const ID_t id);
  void Gear_delete_all(void);
  void Gear_draw(const ID_t id, const GLenum drawMode, const GLuint instances);
  void Gear_use_BO(const ID_t id);
  void Gear_no_BO(const ID_t id);
  void Gear_all_use_BO(void);
  void Gear_all_no_BO(void);

#endif  
  
