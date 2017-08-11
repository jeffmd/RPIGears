/*
* gear.h
*/
#ifndef _GEAR_H_
  #define _GEAR_H_

  #include "GLES/gl.h"
  
  typedef struct {
    GLfloat pos[3];
    GLfloat norm[3];
    GLfloat texCoords[2];
  } vertex_t;
  
  typedef struct {
    vertex_t *vertices;
    GLshort *indices;
    GLfloat color[4];
    int nvertices, nindices;
    GLuint vboId; // ID for vertex buffer object
    GLuint iboId; // ID for index buffer object
    
    GLuint tricount; // number of triangles to draw
    GLvoid *vertex_p; // offset or pointer to first vertex
    GLvoid *normal_p; // offset or pointer to first normal
    GLvoid *index_p;  // offset or pointer to first index
    GLvoid *texCoords_p;  // offset or pointer to first texcoord
  } gear_t;

  extern gear_t* gear( const GLfloat inner_radius, const GLfloat outer_radius,
                       const GLfloat width, const GLint teeth,
                       const GLfloat tooth_depth, const GLfloat color[]);
  
  extern void set_gear_va_ptrs(gear_t *gear);
  extern void make_gear_vbo(gear_t *gear);
  extern void free_gear(gear_t *gear);
  extern void gear_use_vbo(gear_t *gear);

#endif  
  
