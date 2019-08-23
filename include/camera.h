/*
* camera.h
*/

#ifndef _CAMERA_H_
  #define _CAMERA_H_
  

GLfloat camera_z(void);
GLfloat camera_x(void);
GLfloat camera_y(void);
GLfloat camera_rotx(void);
GLfloat camera_roty(void);
GLfloat camera_rotz(void);
void camera_ProjectionMatrix(GLfloat *md);
GLfloat *camera_ProjectionMatrixPtr(void);
void camera_init_ProjectionMatrix(const float aspectratio);
GLfloat *camera_view_matrix(void);
GLboolean camera_isDirty(void);
void camera_init(void);
  
  
#endif
