/*
* camera.h
*/

#ifndef _CAMERA_H_
  #define _CAMERA_H_
  
#include "GLES/gl.h"
  
GLfloat camera_viewDist(void);
GLfloat camera_viewX(void);
GLfloat camera_viewY(void);
GLfloat camera_view_rotx(void);
GLfloat camera_view_roty(void);
GLfloat camera_view_rotz(void);
void change_viewDist(const float val);
void change_viewX(const float val);
void change_viewY(const float val);
void camera_ProjectionMatrix(GLfloat *md);
GLfloat *camera_ProjectionMatrixPtr(void);
void init_ProjectionMatrix(const float aspectratio);
GLfloat *camera_view_matrix(void);
GLboolean camera_isDirty(void);
void init_camera(void);
  
  
#endif
