/*
* gear.h
*/
#ifndef _GEAR_H_
  #define _GEAR_H_

  typedef struct gear_t gear_t;
  
  gear_t *gear( const GLfloat inner_radius, const GLfloat outer_radius,
                       const GLfloat width, const GLint teeth,
                       const GLfloat tooth_depth,
                       const GLfloat color[]);
  
  void gear_delete(gear_t *gear);
  void gear_draw(gear_t *gear, const GLenum drawMode, const GLuint instances);
  void gear_use_BO(gear_t* gear);
  void gear_no_BO(gear_t* gear);

#endif  
  
