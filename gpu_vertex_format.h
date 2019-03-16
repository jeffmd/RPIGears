// gpu_vertex_format.h

#ifndef _GPU_VERTEX_FORMAT_H_
  #define _GPU_VERTEX_FORMAT_H_

  typedef struct VertFormat VertFormat;
  
  void GPU_vertex_format_init(VertFormat *vformat);
  VertFormat *GPU_vertex_format_create(void);
  void GPU_vertex_format_delete(VertFormat *vformat);
  void GPU_vertex_format_add_attribute(VertFormat *vformat, const char *name,
    const GLint size, const GLenum type);
  GLuint GPU_vertex_format_stride(VertFormat *vformat);
  GLuint GPU_vertex_format_offset(VertFormat *vformat, const GLuint idx);
  GLuint GPU_vertex_format_attribute_count(VertFormat *vformat);
  void GPU_vertex_format_add_4(VertFormat *vformat, const GLuint attribute_id,
    GLvoid *data, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4);
  void GPU_vertex_format_bind(VertFormat *vformat, GLvoid *data);
#endif
