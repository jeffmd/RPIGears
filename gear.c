/*
 * gear.c
*/

#include <stdlib.h>
#include <math.h> 
#include <string.h>
#include <stdio.h>

#include "gles3.h"

#include "fp16.h"
#include "gpu_shader_interface.h"

typedef struct {
  GLfloat pos[3];
  GLshort norm[3];
  GLshort texCoords[2];
} vertex_t;

typedef struct {
  vertex_t *vertices;
  GLshort *indices;
  GLfloat color[4];
  int nvertices, nindices;
  GLuint vboId; // ID for vertex buffer object
  GLuint iboId; // ID for index buffer object
  GLuint vaoId; // ID for vertex array object
  
  GLuint tricount; // number of triangles to draw
  GLvoid *vertex_p; // offset or pointer to first vertex
  GLvoid *normal_p; // offset or pointer to first normal
  GLvoid *index_p;  // offset or pointer to first index
  GLvoid *texCoords_p;  // offset or pointer to first texcoord
} gear_t;


static int gearID = 0;
static gear_t gears[3];


// setup pointers/offsets for draw operations
void set_gear_va_ptrs(const int gearid)
{
  gear_t *gear = &gears[gearid - 1];
  // for Vertex Array use pointers to where the buffer starts
  gear->vertex_p = gear->vertices[0].pos;
  gear->normal_p = gear->vertices[0].norm;
  gear->texCoords_p = gear->vertices[0].texCoords;
  gear->index_p = gear->indices;
}
  

void make_gear_vbo(const int gearid)
{
  gear_t* gear = &gears[gearid - 1];
  // for VBO use offsets into the buffer object
  gear->vertex_p = 0;
  gear->normal_p = (GLvoid *)sizeof(gear->vertices[0].pos);
  gear->texCoords_p = (GLvoid *)(sizeof(gear->vertices[0].pos) + sizeof(gear->vertices[0].norm));
  gear->index_p = 0;
  
  // setup the vertex buffer that will hold the vertices , uv, and normals 
  glGenBuffers(1, &gear->vboId);
  glBindBuffer(GL_ARRAY_BUFFER, gear->vboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * gear->nvertices, gear->vertices, GL_STATIC_DRAW);
  
  // setup the index buffer that will hold the indices
  glGenBuffers(1, &gear->iboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gear->iboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * gear->nindices, gear->indices, GL_STATIC_DRAW);
   	
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
  GLshort ix0, ix1, ix2, ix3, ix4;
  vertex_t *vt, *nm, *tx;
  GLshort *ix;
  
  gear_t *gear = &gears[gearID++];
  
  gear->nvertices = teeth * 38;
  gear->nindices = teeth * 64 * 3;
  gear->vertices = calloc(gear->nvertices, sizeof(vertex_t));
  gear->indices = calloc(gear->nindices, sizeof(GLshort));
  memcpy(&gear->color[0], &color[0], sizeof(GLfloat) * 4);

  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.0;
  r2 = outer_radius + tooth_depth / 2.0;
  da = 2.0 * M_PI / teeth / 4.0;

  vt = gear->vertices;
  nm = gear->vertices;
  tx = gear->vertices;
  ix = gear->indices;

#define VERTEX(x,y,z) ((vt->pos[0] = x),(vt->pos[1] = y),(vt->pos[2] = z), \
    (tx->texCoords[0] = f16(x / r2 * 0.8 + 0.5)),(tx->texCoords[1] = f16(y / r2 * 0.8 + 0.5)), (tx++), \
    (vt++ - gear->vertices))
#define NORMAL(x,y,z) ((nm->norm[0] = f16(x)),(nm->norm[1] = f16(y)),(nm->norm[2] = f16(z)), \
                       (nm++))
#define INDEX(a,b,c) ((*ix++ = a),(*ix++ = b),(*ix++ = c))

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
    ix0 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          width * 0.5);
    ix1 = VERTEX(r0 * cos_ta,          r0 * sin_ta,          width * 0.5);
    ix2 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      width * 0.5);
    ix3 = VERTEX(r0 * cos_ta_4da,      r0 * sin_ta_4da,      width * 0.5);
    ix4 = VERTEX(r1 * cos_ta_4da,      r1 * sin_ta_4da,      width * 0.5);
    for (j = 0; j < 5; j++) {
      NORMAL(0.0,                  0.0,                  1.0);
    }
    INDEX(ix0, ix2, ix1);
    INDEX(ix1, ix2, ix3);
    INDEX(ix2, ix4, ix3);

    /* front sides of teeth */
    ix0 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          width * 0.5);
    ix1 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      width * 0.5);
    ix2 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      width * 0.5);
    ix3 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      width * 0.5);
    for (j = 0; j < 4; j++) {
      NORMAL(0.0,                  0.0,                  1.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    /* back face */
    ix0 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          -width * 0.5);
    ix1 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      -width * 0.5);
    ix2 = VERTEX(r0 * cos_ta,          r0 * sin_ta,          -width * 0.5);
    ix3 = VERTEX(r1 * cos_ta_4da,      r1 * sin_ta_4da,      -width * 0.5);
    ix4 = VERTEX(r0 * cos_ta_4da,      r0 * sin_ta_4da,      -width * 0.5);
    for (j = 0; j < 5; j++) {
      NORMAL(0.0,                  0.0,                  -1.0);
    }
    INDEX(ix0, ix2, ix1);
    INDEX(ix1, ix2, ix3);
    INDEX(ix2, ix4, ix3);
    
 /* back sides of teeth */
    ix0 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      -width * 0.5);
    ix1 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      -width * 0.5);
    ix2 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          -width * 0.5);
    ix3 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      -width * 0.5);

    for (j = 0; j < 4; j++) {
      NORMAL(0.0,                  0.0,                  -1.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);

    /* draw outward faces of teeth */
    ix0 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          width * 0.5);
    ix1 = VERTEX(r1 * cos_ta,          r1 * sin_ta,          -width * 0.5);
    ix2 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      width * 0.5);
    ix3 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      -width * 0.5);

    for (j = 0; j < 4; j++) {
      NORMAL(v1,                   -u1,                  0.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    ix0 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      width * 0.5);
    ix1 = VERTEX(r2 * cos_ta_1da,      r2 * sin_ta_1da,      -width * 0.5);
    ix2 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      width * 0.5);
    ix3 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      -width * 0.5);
    for (j = 0; j < 4; j++) {
      NORMAL(cos_ta,               sin_ta,               0.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    ix0 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      width * 0.5);
    ix1 = VERTEX(r2 * cos_ta_2da,      r2 * sin_ta_2da,      -width * 0.5);
    ix2 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      width * 0.5);
    ix3 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      -width * 0.5);
    for (j = 0; j < 4; j++) {
      NORMAL(v2,                   -u2,                  0.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
    ix0 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      width * 0.5);
    ix1 = VERTEX(r1 * cos_ta_3da,      r1 * sin_ta_3da,      -width * 0.5);
    ix2 = VERTEX(r1 * cos_ta_4da,      r1 * sin_ta_4da,      width * 0.5);
    ix3 = VERTEX(r1 * cos_ta_4da,      r1 * sin_ta_4da,      -width * 0.5);
    for (j = 0; j < 4; j++) {
      NORMAL(cos_ta,               sin_ta,               0.0);
    }
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
 /* draw inside radius cylinder */
    ix0 = VERTEX(r0 * cos_ta,          r0 * sin_ta,          -width * 0.5);
    ix1 = VERTEX(r0 * cos_ta,          r0 * sin_ta,          width * 0.5);
    ix2 = VERTEX(r0 * cos_ta_4da,      r0 * sin_ta_4da,      -width * 0.5);
    ix3 = VERTEX(r0 * cos_ta_4da,      r0 * sin_ta_4da,      width * 0.5);
    NORMAL(-cos_ta,              -sin_ta,              0.0);
    NORMAL(-cos_ta,              -sin_ta,              0.0);
    NORMAL(-cos_ta_4da,          -sin_ta_4da,          0.0);
    NORMAL(-cos_ta_4da,          -sin_ta_4da,          0.0);
    INDEX(ix0, ix1, ix2);
    INDEX(ix1, ix3, ix2);
  }

  gear->tricount = gear->nindices / 3;
  
  
  // if VBO enabled then set them up for each gear
  if (useVBO) {
    make_gear_vbo(gearID);
  }
  else {
    set_gear_va_ptrs(gearID);
  }

  return gearID;
}

void gear_use_vbo(const int gearid)
{
  gear_t* gear = &gears[gearid - 1];
  glBindBuffer(GL_ARRAY_BUFFER, gear->vboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gear->iboId);
  //void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
  //glUnmapBuffer(GL_ARRAY_BUFFER);
}

void gear_vbo_off(void)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void free_gear(const int gearid)
{
  gear_t* gear = &gears[gearid - 1];
   if (gear) {
	   if (gear->vboId) {
	     glDeleteBuffers(1, &gear->vboId);
	   }
	   if (gear->iboId) {
	     glDeleteBuffers(1, &gear->iboId);
	   }
     free(gear->vertices);
     free(gear->indices);
   }
}

void gear_draw(const int gearid, const GLenum drawMode, const GLuint MaterialColor_location)
{
  gear_t* gear = &gears[gearid - 1];
  /* Set the gear color */
  glUniform4fv(MaterialColor_location, 1, gear->color);
  
  glBindVertexArray(gear->vaoId);
  
  glDrawElements(drawMode, gear->tricount, GL_UNSIGNED_SHORT,
                 gear->index_p);
  
}

void gear_setVAO(const int gearid, const int useVBO)
{
  gear_t* gear = &gears[gearid - 1];
  
  glGenVertexArrays(1, &gear->vaoId);
  glBindVertexArray(gear->vaoId);
  
  if (useVBO) gear_use_vbo(gearid);
  
  /* Set up the position of the attributes in the vertex buffer object */
  // setup where vertex data is
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(get_active_attribute_location("position"), 3, GL_FLOAT, GL_FALSE,
       sizeof(vertex_t), gear->vertex_p);
  // setup where normal data is
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(get_active_attribute_location("normal"), 3, GL_HALF_FLOAT_OES, GL_FALSE,
       sizeof(vertex_t), gear->normal_p);
  // setup where uv data is
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(get_active_attribute_location("uv"), 2, GL_HALF_FLOAT_OES, GL_FALSE,
       sizeof(vertex_t), gear->texCoords_p);
	
}
