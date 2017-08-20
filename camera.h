/*
* camera.h
*/

#ifndef _CAMERA_H_
  #define _CAMERA_H_
  
#include "GLES/gl.h"
  
extern GLfloat camera_viewDist(void);
extern GLfloat camera_viewX(void);
extern GLfloat camera_viewY(void);
extern GLfloat camera_view_rotx(void);
extern GLfloat camera_view_roty(void);
extern GLfloat camera_view_rotz(void);
extern void change_viewDist(const float val);
extern void change_viewX(const float val);
extern void change_viewY(const float val);
extern void camera_ProjectionMatrix(GLfloat *md);
extern void init_ProjectionMatrix(const float aspectratio);
extern void build_view_matrix(GLfloat *view_transform);
extern void init_camera(void);
  
  
#endif
