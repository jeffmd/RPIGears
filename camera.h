/*
* camera.h
*/

#ifndef _CAMERA_H_
  #define _CAMERA_H_
  

GLfloat camera_viewDist(void);
GLfloat camera_viewX(void);
GLfloat camera_viewY(void);
GLfloat camera_view_rotx(void);
GLfloat camera_view_roty(void);
GLfloat camera_view_rotz(void);
void camera_change_viewDist(const float val);
void camera_change_viewX(const float val);
void camera_change_viewY(const float val);
void camera_ProjectionMatrix(GLfloat *md);
GLfloat *camera_ProjectionMatrixPtr(void);
void camera_init_ProjectionMatrix(const float aspectratio);
GLfloat *camera_view_matrix(void);
GLboolean camera_isDirty(void);
void camera_init(void);
  
  
#endif
