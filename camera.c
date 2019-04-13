/*
* camera.c
*/

#include <stdio.h>
#include "gles3.h"

#include "matrix_math.h"


typedef struct {
  GLfloat z;
  GLfloat x;
  GLfloat y;
  GLfloat view_rotx;
  GLfloat view_roty;
  GLfloat view_rotz;
  GLboolean dirty;
  GLfloat ViewMatrix[16];
  GLfloat ProjectionMatrix[16];

} CAMERA_T;

static CAMERA_T _camera;
static CAMERA_T * const camera = &_camera; 

GLfloat camera_z(void)
{
  return camera->z;
}

GLfloat camera_x(void)
{
  return camera->x;
}

GLfloat camera_y(void)
{
  return camera->y;
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

void camera_change_z(const float val)
{
  camera->z += val;  
  camera->dirty = GL_TRUE;
}

void camera_change_x(const float val)
{
  camera->x += val;
  camera->dirty = GL_TRUE;
}

void camera_change_y(const float val)
{
  camera->y += val;  
  camera->dirty = GL_TRUE;
}

void camera_ProjectionMatrix(GLfloat *md)
{
   m4x4_copy(md, camera->ProjectionMatrix);
}

GLfloat *camera_ProjectionMatrixPtr(void)
{
   return camera->ProjectionMatrix;
}

void camera_init_ProjectionMatrix(const float aspectratio)
{
   m4x4_perspective(camera->ProjectionMatrix, 35.0, aspectratio, 1.0, 100.0);
}

GLboolean camera_isDirty(void)
{
   return camera->dirty;	
}

GLfloat *camera_view_matrix(void)
{
   if (camera->dirty == GL_TRUE) {
	   printf("camera Recalc\n");
	   m4x4_identity(camera->ViewMatrix);
	   /* Translate and rotate the view */
	   m4x4_translate(camera->ViewMatrix, camera->x, camera->y, camera->z);
	   m4x4_rotate(camera->ViewMatrix, camera->view_rotx, 1, 0, 0);
	   m4x4_rotate(camera->ViewMatrix, camera->view_roty, 0, 1, 0);
	   m4x4_rotate(camera->ViewMatrix, camera->view_rotz, 0, 0, 1);
	   camera->dirty = GL_FALSE;
	}
	
	return camera->ViewMatrix;
}

void camera_init(void)
{
  //memset( camera, 0, sizeof( *camera ) );

  camera->z = -45.0f;
  camera->x = 2.0f;
  camera->y = 0.0f;
  camera->view_rotx = 15.0f;
  camera->view_roty = 25.0f;
  camera->view_rotz = 0.0f;
  camera->dirty = GL_TRUE;
  
}
