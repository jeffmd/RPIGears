/*
* gear.h
*/

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

