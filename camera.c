/*
* camera.c
*/

#include "matrix_math.h"
#include "string.h"

#include "GLES2/gl2.h"

typedef struct {
  GLfloat viewDist;
  GLfloat viewX;
  GLfloat viewY;
  GLfloat view_rotx;
  GLfloat view_roty;
  GLfloat view_rotz;

  GLfloat ProjectionMatrix[16];

} CAMERA_T;

static CAMERA_T _camera;
static CAMERA_T * const camera = &_camera; 

GLfloat camera_viewDist(void)
{
  return camera->viewDist;
}

GLfloat camera_viewX(void)
{
  return camera->viewX;
}

GLfloat camera_viewY(void)
{
  return camera->viewY;
}

GLfloat camera_view_rotx(void)
{
  return camera->view_rotx;
}

GLfloat camera_view_roty(void)
{
  return camera->view_roty;
}

GLfloat camera_view_rotz(void)
{
  return camera->view_rotz;
}

void change_viewDist(const float val)
{
  camera->viewDist += val;  
}

void change_viewX(const float val)
{
  camera->viewX += val;
}

void change_viewY(const float val)
{
  camera->viewY += val;  
}

void camera_ProjectionMatrix(GLfloat *md)
{
   m4x4_copy(md, camera->ProjectionMatrix);
}

void init_ProjectionMatrix(const float aspectratio)
{
   m4x4_perspective(camera->ProjectionMatrix, 45.0, aspectratio, 1.0, 100.0);
}

void build_view_matrix(GLfloat *view_transform)
{
   m4x4_identity(view_transform);
   /* Translate and rotate the view */
   m4x4_translate(view_transform, camera->viewX, camera->viewY, camera->viewDist);
   m4x4_rotate(view_transform, camera->view_rotx, 1, 0, 0);
   m4x4_rotate(view_transform, camera->view_roty, 0, 1, 0);
   m4x4_rotate(view_transform, camera->view_rotz, 0, 0, 1);
}

void init_camera(void)
{
  memset( camera, 0, sizeof( *camera ) );

  camera->viewDist = -38.0f;
  camera->viewX = -8.0f;
  camera->viewY = -7.0f;
  camera->view_rotx = 25.0f;
  camera->view_roty = 30.0f;
  camera->view_rotz = 0.0f;
  
}
