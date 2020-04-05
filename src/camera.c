/*
* camera.c
*/

#include <stdio.h>

#include "gles3.h"
#include "matrix_math.h"
#include "key_input.h"

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

static GLfloat tm[16];
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

static void camera_change_z(const float val)
{
  camera->z += val;  
  camera->dirty = GL_TRUE;
}

static void camera_change_x(const float val)
{
  camera->x += val;
  camera->dirty = GL_TRUE;
}

static void camera_change_y(const float val)
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
   m4x4_perspective(camera->ProjectionMatrix, 35.0, aspectratio, 0.0, 100.0);
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
    m4x4_translate(tm, camera->ViewMatrix, camera->x, camera->y, camera->z);
    m4x4_rotate(camera->ViewMatrix, tm, camera->view_rotx, 1, 0, 0);
    m4x4_rotate(tm, camera->ViewMatrix, camera->view_roty, 0, 1, 0);
    m4x4_rotate(camera->ViewMatrix, tm, camera->view_rotz, 0, 0, 1);
    camera->dirty = GL_FALSE;
  }

  return camera->ViewMatrix;
}

static void camera_key_reverse(const short souce_id, const short destination_id)
{
  key_input_set_update(camera_change_z, -1.0f);
}

static void camera_key_forward(const short souce_id, const short destination_id)
{
  key_input_set_update(camera_change_z, 1.0f);
}

static void camera_key_right(const short souce_id, const short destination_id)
{
  key_input_set_update(camera_change_x, -1.0f);
}

static void camera_key_left(const short souce_id, const short destination_id)
{
  key_input_set_update(camera_change_x, 1.0f);
}
          
static void camera_key_down(const short souce_id, const short destination_id)
{
  key_input_set_update(camera_change_y, 1.0f);
}

static void camera_key_up(const short souce_id, const short destination_id)
{
  key_input_set_update(camera_change_y, -1.0f);
}

void camera_init(void)
{
  camera->z = -45.0f;
  camera->x = 2.0f;
  camera->y = 0.0f;
  camera->view_rotx = 15.0f;
  camera->view_roty = 25.0f;
  camera->view_rotz = 0.0f;
  camera->dirty = GL_TRUE;
  
  key_add_action('r', camera_key_reverse, "move camera back from gears");
  key_add_action('f', camera_key_forward, "move camera toward gears");
  key_add_action('a', camera_key_left, "move camera left");
  key_add_action('d', camera_key_right, "move camera right");
  key_add_action('w', camera_key_up, "move camera up");
  key_add_action('s', camera_key_down, "move camera down");
  
}
