/*
* camera.c
*/

#include <stdio.h>

#include "gles3.h"
#include "matrix_math.h"
#include "key_input.h"
#include "static_array.h"

typedef struct {
  uint8_t active;
  GLfloat z;
  GLfloat x;
  GLfloat y;
  GLfloat view_rotx;
  GLfloat view_roty;
  GLfloat view_rotz;
  GLboolean dirty;
  GLfloat ViewMatrix[16];
  GLfloat ProjectionMatrix[16];

} Camera;

#define CAMERA_MAX_COUNT 10

static GLfloat tm[16];
static Camera cameras[CAMERA_MAX_COUNT];
static short next_deleted_camera;


static Camera *active_camera;
static short default_camera;

static inline short find_deleted_camera(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_camera, cameras, CAMERA_MAX_COUNT, Camera, "Camera");
}

static Camera *get_camera(short id)
{
  if ((id < 0) | (id >= CAMERA_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Camera id, using default id: 0\n");
  }
    
  return cameras + id;
}

static void camera_init(Camera *camera)
{
  camera->z = -45.0f;
  camera->x = 2.0f;
  camera->y = 0.0f;
  camera->view_rotx = 15.0f;
  camera->view_roty = 25.0f;
  camera->view_rotz = 0.0f;
  camera->dirty = GL_TRUE;
}

short Camera_create(void)
{
  const short id = find_deleted_camera();
  Camera *const camera = get_camera(id);
  camera->active = 1;
  camera_init(camera);

  return id;
}

static void camera_change_z(const float val)
{
  active_camera->z += val;
  active_camera->dirty = GL_TRUE;
}

static void camera_change_x(const float val)
{
  active_camera->x += val;
  active_camera->dirty = GL_TRUE;
}

static void camera_change_y(const float val)
{
  active_camera->y += val;  
  active_camera->dirty = GL_TRUE;
}

static void camera_key_reverse(const short souce_id, const short destination_id)
{
  Key_input_set_update(camera_change_z, -1.0f);
}

static void camera_key_forward(const short souce_id, const short destination_id)
{
  Key_input_set_update(camera_change_z, 1.0f);
}

static void camera_key_right(const short souce_id, const short destination_id)
{
  Key_input_set_update(camera_change_x, -1.0f);
}

static void camera_key_left(const short souce_id, const short destination_id)
{
  Key_input_set_update(camera_change_x, 1.0f);
}
          
static void camera_key_down(const short souce_id, const short destination_id)
{
  Key_input_set_update(camera_change_y, 1.0f);
}

static void camera_key_up(const short souce_id, const short destination_id)
{
  Key_input_set_update(camera_change_y, -1.0f);
}

static void camera_set_keys(void)
{
  Key_add_action('r', camera_key_reverse, "move camera back from gears");
  Key_add_action('f', camera_key_forward, "move camera toward gears");
  Key_add_action('a', camera_key_left, "move camera left");
  Key_add_action('d', camera_key_right, "move camera right");
  Key_add_action('w', camera_key_up, "move camera up");
  Key_add_action('s', camera_key_down, "move camera down");  
}

static short get_default_camera(void)
{
  if (!default_camera) {
    default_camera = Camera_create();
    camera_set_keys();
  }

  return default_camera;
}

static Camera *get_active_camera(void)
{
  if (!active_camera) {
    active_camera = get_camera(get_default_camera());
  }

  return active_camera;
}

GLfloat Camera_z(void)
{
  return get_active_camera()->z;
}

GLfloat Camera_x(void)
{
  return get_active_camera()->x;
}

GLfloat Camera_y(void)
{
  return get_active_camera()->y;
}

GLfloat Camera_view_rotx(void)
{
  return get_active_camera()->view_rotx;
}

GLfloat Camera_view_roty(void)
{
  return get_active_camera()->view_roty;
}

GLfloat Camera_view_rotz(void)
{
  return get_active_camera()->view_rotz;
}

void Camera_ProjectionMatrix(GLfloat *md)
{
   m4x4_copy(md, get_active_camera()->ProjectionMatrix);
}

GLfloat *Camera_ProjectionMatrixPtr(void)
{
   return get_active_camera()->ProjectionMatrix;
}

void Camera_init_ProjectionMatrix(const float aspectratio)
{
   m4x4_perspective(get_active_camera()->ProjectionMatrix, 35.0, aspectratio, 0.0, 100.0);
}

GLboolean Camera_isDirty(void)
{
   return get_active_camera()->dirty;
}

GLfloat *Camera_view_matrix(void)
{
  Camera *camera = get_active_camera();

  if (camera->dirty == GL_TRUE) {
    //printf("camera Recalc\n");
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

void Camera_set_active(const short id)
{
  active_camera = get_camera(id);
}

