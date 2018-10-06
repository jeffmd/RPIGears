/*
* camera.c
*/

#include <stdio.h>
#include "GLES2/gl2.h"

#include "matrix_math.h"


typedef struct {
  GLfloat viewDist;
  GLfloat viewX;
  GLfloat viewY;
  GLfloat view_rotx;
  GLfloat view_roty;
  GLfloat view_rotz;
  GLboolean dirty;
  GLfloat ViewMatrix[16];
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
  camera->dirty = GL_TRUE;
}

void change_viewX(const float val)
{
  camera->viewX += val;
  camera->dirty = GL_TRUE;
}

void change_viewY(const float val)
{
  camera->viewY += val;  
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

void init_ProjectionMatrix(const float aspectratio)
{
   m4x4_perspective(camera->ProjectionMatrix, 45.0, aspectratio, 1.0, 100.0);
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
	   m4x4_translate(camera->ViewMatrix, camera->viewX, camera->viewY, camera->viewDist);
	   m4x4_rotate(camera->ViewMatrix, camera->view_rotx, 1, 0, 0);
	   m4x4_rotate(camera->ViewMatrix, camera->view_roty, 0, 1, 0);
	   m4x4_rotate(camera->ViewMatrix, camera->view_rotz, 0, 0, 1);
	   camera->dirty = GL_FALSE;
	}
	
	return camera->ViewMatrix;
}

void init_camera(void)
{
  //memset( camera, 0, sizeof( *camera ) );

  camera->viewDist = -35.0f;
  camera->viewX = 2.0f;
  camera->viewY = 0.0f;
  camera->view_rotx = 15.0f;
  camera->view_roty = 25.0f;
  camera->view_rotz = 0.0f;
  camera->dirty = GL_TRUE;
  
}
