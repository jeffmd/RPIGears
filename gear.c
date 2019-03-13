/*
 * gear.c
*/

#include <stdlib.h>
#include <math.h> 
#include <string.h>
#include <stdio.h>

#include "gles3.h"

#include "fp16.h"
#include "gpu_vertex_buffer.h"
#include "gpu_vertex_format.h"
#include "gpu_index_buffer.h"
#include "gpu_batch.h"

typedef struct {
  GLuint nvertices, nindices;
  GLfloat color[4];
  uint8_t ubuffId; // ID for uniform buffer
  uint8_t batch_id; 
} gear_t;

#define GEARS_MAX_COUNT 3

enum {
  ATTR_POSITION,
  ATTR_NORMAL,
  ATTR_UV,
};
  
static int gearID = 0;
static gear_t gears[GEARS_MAX_COUNT];
static uint8_t vformat_id = 0;
/**
 
  build a gear wheel.  You'll probably want to call this function when
  building a display list or filling a vertex buffer object
  since we do a lot of trig here.
 
  Input:  inner_radius - radius of hole at center
          outer_radius - radius at center of teeth
          width - width of gear
          teeth - number of teeth
          tooth_depth - depth of tooth
 
 **/
int gear( const GLfloat inner_radius, const GLfloat outer_radius,
                     const GLfloat width, const GLint teeth,
                     const GLfloat tooth_depth, const GLfloat color[],
                     const int useVBO)
{
  GLint i, j;
  GLfloat r0, r1, r2;
  GLfloat ta, da;
  GLfloat u1, v1, u2, v2, len;
  GLfloat cos_ta, cos_ta_1da, cos_ta_2da, cos_ta_3da, cos_ta_4da;
  GLfloat sin_ta, sin_ta_1da, sin_ta_2da, sin_ta_3da, sin_ta_4da;
  GLshort ix0, ix1, ix2, ix3, ix4, idx;
  
  gear_t *gear = &gears[gearID++];
  
  const int nvertices = teeth * 38;
  const int nindices = teeth * 64 * 3;

  memcpy(&gear->color[0], &color[0], sizeof(GLfloat) * 4);
  
  gear->batch_id = GPU_batch_create();

  const GLuint ibuffId = GPU_indexbuf_create();
  GPU_batch_set_index_buffer(gear->batch_id, ibuffId);
  GPU_batch_set_indices_draw_count(gear->batch_id, nindices);
  GPU_indexbuf_begin_update(ibuffId, nindices);
    
  if (!vformat_id) {
    vformat_id = GPU_vertex_format_create();
    GPU_vertex_format_add_attribute(vformat_id, "position", 3, GL_FLOAT);
    GPU_vertex_format_add_attribute(vformat_id, "normal", 3, GL_HALF_FLOAT_OES);
    GPU_vertex_format_add_attribute(vformat_id, "uv", 2, GL_HALF_FLOAT_OES);
  }
  
  const GLuint vbuffId = GPU_vertbuf_create();
  GPU_batch_set_vertex_buffer(gear->batch_id, vbuffId);
  GPU_batch_set_vertices_draw_count(gear->batch_id, nvertices);
  GPU_vertbuf_set_vertex_format(vbuffId, vformat_id);
  GPU_vertbuf_begin_update(vbuffId, nvertices);
  
  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.0;
  r2 = outer_radius + tooth_depth / 2.0;
  da = 2.0 * M_PI / teeth / 4.0;

  idx = 0;
  
#define VERTEX(x,y,z) (GPU_vertbuf_add_3(vbuffId, ATTR_POSITION, x, y, z), \
    GPU_vertbuf_add_2(vbuffId, ATTR_UV, (x / r2 * 0.8 + 0.5), (y / r2 * 0.8 + 0.5)), \
    idx++)
#define NORMAL(x,y,z) GPU_vertbuf_add_3(vbuffId, ATTR_NORMAL, x, y, z)
#define INDEX(a,b,c) GPU_indexbuf_add_3(ibuffId, a, b, c)

  for (i = 0; i < teeth; i++) {
    ta = i * 2.0 * M_PI / teeth;

    cos_ta = cos(ta);
    cos_ta_1da = cos(ta + da);
    cos_ta_2da = cos(ta + 2 * da);
    cos_ta_3da = cos(ta + 3 * da);
    cos_ta_4da = cos(ta + 4 * da);
    sin_ta = sin(ta);
    sin_ta_1da = sin(ta + da);
    sin_ta_2da = sin(ta + 2 * da);
    sin_ta_3da = sin(ta + 3 * da);
    sin_ta_4da = sin(ta + 4 * da);

    u1 = r2 * cos_ta_1da - r1 * cos_ta;
    v1 = r2 * sin_ta_1da - r1 * sin_ta;
    len = sqrt(u1 * u1 + v1 * v1);
    u1 /= len;
    v1 /= len;
    u2 = r1 * cos_ta_3da - r2 * cos_ta_2da;
    v2 = r1 * sin_ta_3da - r2 * sin_ta_2da;

    /* front face */
    ix0 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          width);
    ix1 = VERTEX(r0 * cos_ta,          r0 * sin_ta,          width);
    ix2 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      width);
    ix3 = VERTEX(r0 * cos_ta_4da,      r0 * sin_ta_4da,      width);
    ix4 = VERTEX(r1 * cos_ta_4da,      r1 * sin_ta_4da,      width);
    for (j = 0; j < 5; j++) {
      NORMAL(0.0,                  0.0,                  1.0);
    }
    INDEX(ix0, ix2, ix1);
    INDEX(ix1, ix2, ix3);
    INDEX(ix2, ix4, ix3);

    /* front sides of teeth */
    ix0 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          width);
    ix1 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      width);
    ix2 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      width);
    ix3 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      width);
    for (j = 0; j < 4; j++) {
      NORMAL(0.0,                  0.0,                  1.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    /* back face */
    ix0 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          -width);
    ix1 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      -width);
    ix2 = VERTEX(r0 * cos_ta,          r0 * sin_ta,          -width);
    ix3 = VERTEX(r1 * cos_ta_4da,      r1 * sin_ta_4da,      -width);
    ix4 = VERTEX(r0 * cos_ta_4da,      r0 * sin_ta_4da,      -width);
    for (j = 0; j < 5; j++) {
      NORMAL(0.0,                  0.0,                  -1.0);
    }
    INDEX(ix0, ix2, ix1);
    INDEX(ix1, ix2, ix3);
    INDEX(ix2, ix4, ix3);
    
 /* back sides of teeth */
    ix0 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      -width);
    ix1 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      -width);
    ix2 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          -width);
    ix3 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      -width);

    for (j = 0; j < 4; j++) {
      NORMAL(0.0,                  0.0,                  -1.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);

    /* draw outward faces of teeth */
    ix0 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          width);
    ix1 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          -width);
    ix2 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      width);
    ix3 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      -width);

    for (j = 0; j < 4; j++) {
      NORMAL(v1,                   -u1,                  0.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    ix0 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      width);
    ix1 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      -width);
    ix2 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      width);
    ix3 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      -width);
    for (j = 0; j < 4; j++) {
      NORMAL(cos_ta,               sin_ta,               0.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    ix0 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      width);
    ix1 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      -width);
    ix2 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      width);
    ix3 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      -width);
    for (j = 0; j < 4; j++) {
      NORMAL(v2,                   -u2,                  0.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    ix0 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      width);
    ix1 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      -width);
    ix2 = VERTEX(r1 * cos_ta_4da,      r1 * sin_ta_4da,      width);
    ix3 = VERTEX(r1 * cos_ta_4da,      r1 * sin_ta_4da,      -width);
    for (j = 0; j < 4; j++) {
      NORMAL(cos_ta,               sin_ta,               0.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
 /* draw inside radius cylinder */
    ix0 = VERTEX(r0 * cos_ta,          r0 * sin_ta,          -width);
    ix1 = VERTEX(r0 * cos_ta,          r0 * sin_ta,          width);
    ix2 = VERTEX(r0 * cos_ta_4da,      r0 * sin_ta_4da,      -width);
    ix3 = VERTEX(r0 * cos_ta_4da,      r0 * sin_ta_4da,      width);
    NORMAL(-cos_ta,              -sin_ta,              0.0);
    NORMAL(-cos_ta,              -sin_ta,              0.0);
    NORMAL(-cos_ta_4da,          -sin_ta_4da,          0.0);
    NORMAL(-cos_ta_4da,          -sin_ta_4da,          0.0);
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
  }

  // if VBO enabled then set them up for each gear
  if (useVBO) {
    GPU_indexbuf_use_VBO(ibuffId);
    GPU_vertbuf_use_VBO(vbuffId);
  }
  
  return gearID;
}

void gear_vbo_off(void)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void free_gear(const int gearid)
{
  gear_t* gear = &gears[gearid - 1];

  GPU_batch_delete(gear->batch_id, 1);
}

void gear_draw(const int gearid, const GLenum drawMode, const GLuint MaterialColor_location, const GLuint instances)
{
  gear_t* gear = &gears[gearid - 1];
  /* Set the gear color */
  glUniform4fv(MaterialColor_location, 1, gear->color);
  
  GPU_batch_draw(gear->batch_id, drawMode, instances);
}
