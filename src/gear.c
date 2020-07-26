/*
 * gear.c
*/

#include <stdlib.h>
#include <math.h> 
#include <string.h>
#include <stdio.h>

#include "gles3.h"

#include "gpu_vertex_format.h"
#include "gpu_vertex_buffer.h"
#include "gpu_index_buffer.h"
#include "gpu_uniform_buffer.h"
#include "gpu_batch.h"

#include "static_array.h"

typedef struct {
  uint8_t active;
  short batch; 
  GLfloat color[4];
} Gear;

enum {
  ATTR_POSITION,
  ATTR_NORMAL,
  ATTR_UV,
};

#define GEAR_MAX_COUNT 10
  
static Gear gears[GEAR_MAX_COUNT];
static short next_deleted_gear;

static short vformat = 0;

static inline short find_deleted_gear(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_gear, gears, GEAR_MAX_COUNT, Gear, "Gear");
}

static Gear *get_gear(short id)
{
  if ((id < 0) | (id >= GEAR_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Gear id, using default id: 0\n");
  }
    
  return gears + id;
}


static short gear_vformat(void)
{
  if (!vformat) {
    vformat = GPU_vertex_format_create();
    GPU_vertex_format_add_attribute(vformat, "position", 3, GL_FLOAT);
    GPU_vertex_format_add_attribute(vformat, "normal", 3, GL_HALF_FLOAT_OES);
    GPU_vertex_format_add_attribute(vformat, "uv", 2, GL_HALF_FLOAT_OES);
  }
  return vformat;
}

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
short Gear_create( const GLfloat inner_radius, const GLfloat outer_radius,
          const GLfloat width, const GLint teeth,
          const GLfloat tooth_depth,
          const GLfloat color[4])
{
  GLint i, j;
  GLfloat r0, r1, r2;
  GLfloat ta, da;
  GLfloat u1, v1, u2, v2, len;
  GLfloat cos_ta, cos_ta_1da, cos_ta_2da, cos_ta_3da, cos_ta_4da;
  GLfloat sin_ta, sin_ta_1da, sin_ta_2da, sin_ta_3da, sin_ta_4da;
  GLfloat r0_cos_ta, r1_cos_ta, r1_sin_ta, r2_cos_ta_1da, r1_cos_ta_3da;
  GLfloat r0_cos_ta_4da, r1_cos_ta_4da, r2_cos_ta_2da, r0_sin_ta;
  GLfloat r1_sin_ta_3da, r0_sin_ta_4da, r1_sin_ta_4da, r2_sin_ta_2da;
  GLfloat r2_sin_ta_1da;
  GLshort ix0, ix1, ix2, ix3, ix4, idx;
  
  const short id = find_deleted_gear();
  Gear *gear = get_gear(id);
  
  const int nvertices = teeth * 38;
  const int nindices = teeth * 64 * 3;

  memcpy(&gear->color[0], &color[0], sizeof(GLfloat) * 4);
  
  gear->batch = GPU_batch_create();

  const short ibuff = GPU_batch_index_buffer(gear->batch);
  GPU_batch_set_indices_draw_count(gear->batch, nindices);
  GPU_indexbuf_set_add_count(ibuff, nindices);
  
  const short ubuff = GPU_batch_uniform_buffer(gear->batch);
  GPU_uniformbuffer_add_4f(ubuff, "MaterialColor", gear->color);
  
  const short vbuff = GPU_batch_vertex_buffer(gear->batch);
  GPU_batch_set_vertices_draw_count(gear->batch, nvertices);
  GPU_vertbuf_set_vertex_format(vbuff, gear_vformat());
  GPU_vertbuf_set_add_count(vbuff, nvertices);
  
  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.0f;
  r2 = outer_radius + tooth_depth / 2.0f;
  da = 2.0f * M_PI / teeth / 4.0f;

  idx = 0;
  
#define VERTEX(x,y,z) (GPU_vertbuf_add_3(vbuff, ATTR_POSITION, x, y, z), \
    GPU_vertbuf_add_2(vbuff, ATTR_UV, (x / r2 * 0.8f + 0.5f), (y / r2 * 0.8f + 0.5f)), \
    idx++)
#define NORMAL(x,y,z) GPU_vertbuf_add_3(vbuff, ATTR_NORMAL, x, y, z)
#define INDEX(a,b,c) GPU_indexbuf_add(ibuff, a); GPU_indexbuf_add(ibuff, b); GPU_indexbuf_add(ibuff, c)

  for (i = 0; i < teeth; i++) {
    ta = i * 2.0f * M_PI / teeth;

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

    r0_cos_ta = r0 * cos_ta;
    r0_sin_ta = r0 * sin_ta;
    r0_cos_ta_4da = r0 * cos_ta_4da;
    r0_sin_ta_4da = r0 * sin_ta_4da;

    r1_cos_ta = r1 * cos_ta;
    r1_sin_ta = r1 * sin_ta;
    r1_sin_ta_3da = r1 * sin_ta_3da;
    r1_sin_ta_4da = r1 * sin_ta_4da;
    r1_cos_ta_3da = r1 * cos_ta_3da;
    r1_cos_ta_4da = r1 * cos_ta_4da;

    r2_cos_ta_1da = r2 * cos_ta_1da;
    r2_cos_ta_2da = r2 * cos_ta_2da;
    r2_sin_ta_1da = r2 * sin_ta_1da;
    r2_sin_ta_2da = r2 * sin_ta_2da;
    
    u1 = r2_cos_ta_1da - r1_cos_ta;
    v1 = r2_sin_ta_1da - r1_sin_ta;
    len = sqrt(u1 * u1 + v1 * v1);
    u1 /= len;
    v1 /= len;
    u2 = r1_cos_ta_3da - r2_cos_ta_2da;
    v2 = r1_sin_ta_3da - r2_sin_ta_2da;

    /* front face */
    ix0 = VERTEX(r1_cos_ta,     r1_sin_ta,     width);
    ix1 = VERTEX(r0_cos_ta,     r0_sin_ta,     width);
    ix2 = VERTEX(r1_cos_ta_3da, r1_sin_ta_3da, width);
    ix3 = VERTEX(r0_cos_ta_4da, r0_sin_ta_4da, width);
    ix4 = VERTEX(r1_cos_ta_4da, r1_sin_ta_4da, width);
    for (j = 0; j < 5; j++) {
      NORMAL(0.0f, 0.0f, 1.0f);
    }
    INDEX(ix0, ix2, ix1);
    INDEX(ix1, ix2, ix3);
    INDEX(ix2, ix4, ix3);

    /* front sides of teeth */
    ix0 = VERTEX(r1_cos_ta,     r1_sin_ta,     width);
    ix1 = VERTEX(r2_cos_ta_1da, r2_sin_ta_1da, width);
    ix2 = VERTEX(r1_cos_ta_3da, r1_sin_ta_3da, width);
    ix3 = VERTEX(r2_cos_ta_2da, r2_sin_ta_2da, width);
    for (j = 0; j < 4; j++) {
      NORMAL(0.0f, 0.0f, 1.0f);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);

    /* back face */
    ix0 = VERTEX(r1_cos_ta,     r1_sin_ta,     -width);
    ix1 = VERTEX(r1_cos_ta_3da, r1_sin_ta_3da, -width);
    ix2 = VERTEX(r0_cos_ta,     r0_sin_ta,     -width);
    ix3 = VERTEX(r1_cos_ta_4da, r1_sin_ta_4da, -width);
    ix4 = VERTEX(r0_cos_ta_4da, r0_sin_ta_4da, -width);
    for (j = 0; j < 5; j++) {
      NORMAL(0.0f, 0.0f, -1.0f);
    }
    INDEX(ix0, ix2, ix1);
    INDEX(ix1, ix2, ix3);
    INDEX(ix2, ix4, ix3);
    
 /* back sides of teeth */
    ix0 = VERTEX(r1_cos_ta_3da, r1_sin_ta_3da, -width);
    ix1 = VERTEX(r2_cos_ta_2da, r2_sin_ta_2da, -width);
    ix2 = VERTEX(r1_cos_ta,     r1_sin_ta,     -width);
    ix3 = VERTEX(r2_cos_ta_1da, r2_sin_ta_1da, -width);

    for (j = 0; j < 4; j++) {
      NORMAL(0.0f, 0.0f, -1.0f);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);

    /* draw outward faces of teeth */
    ix0 = VERTEX(r1_cos_ta,     r1_sin_ta,     width);
    ix1 = VERTEX(r1_cos_ta,     r1_sin_ta,     -width);
    ix2 = VERTEX(r2_cos_ta_1da, r2_sin_ta_1da, width);
    ix3 = VERTEX(r2_cos_ta_1da, r2_sin_ta_1da, -width);

    for (j = 0; j < 4; j++) {
      NORMAL(v1, -u1, 0.0f);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    ix0 = VERTEX(r2_cos_ta_1da, r2_sin_ta_1da, width);
    ix1 = VERTEX(r2_cos_ta_1da, r2_sin_ta_1da, -width);
    ix2 = VERTEX(r2_cos_ta_2da, r2_sin_ta_2da, width);
    ix3 = VERTEX(r2_cos_ta_2da, r2_sin_ta_2da, -width);
    for (j = 0; j < 4; j++) {
      NORMAL(cos_ta, sin_ta, 0.0f);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    ix0 = VERTEX(r2_cos_ta_2da, r2_sin_ta_2da, width);
    ix1 = VERTEX(r2_cos_ta_2da, r2_sin_ta_2da, -width);
    ix2 = VERTEX(r1_cos_ta_3da, r1_sin_ta_3da, width);
    ix3 = VERTEX(r1_cos_ta_3da, r1_sin_ta_3da, -width);
    for (j = 0; j < 4; j++) {
      NORMAL(v2, -u2, 0.0f);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    ix0 = VERTEX(r1_cos_ta_3da, r1_sin_ta_3da, width);
    ix1 = VERTEX(r1_cos_ta_3da, r1_sin_ta_3da, -width);
    ix2 = VERTEX(r1_cos_ta_4da, r1_sin_ta_4da, width);
    ix3 = VERTEX(r1_cos_ta_4da, r1_sin_ta_4da, -width);
    for (j = 0; j < 4; j++) {
      NORMAL(cos_ta, sin_ta, 0.0f);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
 /* draw inside radius cylinder */
    ix0 = VERTEX(r0_cos_ta,     r0_sin_ta,     -width);
    ix1 = VERTEX(r0_cos_ta,     r0_sin_ta,     width);
    ix2 = VERTEX(r0_cos_ta_4da, r0_sin_ta_4da, -width);
    ix3 = VERTEX(r0_cos_ta_4da, r0_sin_ta_4da, width);
    NORMAL(-cos_ta, -sin_ta, 0.0f);
    NORMAL(-cos_ta, -sin_ta, 0.0f);
    NORMAL(-cos_ta_4da, -sin_ta_4da, 0.0f);
    NORMAL(-cos_ta_4da, -sin_ta_4da, 0.0f);
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
  }
  printf("gear vertices: %i\n", idx); 
  return id;
}

void Gear_delete(const short id)
{
  GPU_batch_delete(get_gear(id)->batch, 1);
}

void Gear_draw(const short id, const GLenum drawMode, const GLuint instances)
{
  GPU_batch_draw(get_gear(id)->batch, drawMode, instances);
}

void Gear_use_BO(const short id)
{
  GPU_batch_use_BO(get_gear(id)->batch);
}

void Gear_no_BO(const short id)
{
  GPU_batch_no_BO(get_gear(id)->batch);
}
