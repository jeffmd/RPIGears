/*
* camera.h
*/

#ifndef _CAMERA_H_
  #define _CAMERA_H_
  

GLfloat Camera_z(void);
GLfloat Camera_x(void);
GLfloat Camera_y(void);
GLfloat Camera_rotx(void);
GLfloat Camera_roty(void);
GLfloat Camera_rotz(void);
void Camera_ProjectionMatrix(GLfloat *md);
GLfloat *Camera_ProjectionMatrixPtr(void);
void Camera_init_ProjectionMatrix(const float aspectratio);
GLfloat *Camera_view_matrix(void);
GLboolean Camera_isDirty(void);
ID_t Camera_create(void);
void Camera_set_active(const ID_t id);
  
  
#endif
