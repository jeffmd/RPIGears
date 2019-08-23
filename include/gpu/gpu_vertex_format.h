// gpu_vertex_format.h

#ifndef _GPU_VERTEX_FORMAT_H_
  #define _GPU_VERTEX_FORMAT_H_

  int GPU_vertex_format_create(void);
  void GPU_vertex_format_delete(const int id);
  void GPU_vertex_format_add_attribute(const int id, const char *name,
    const GLint size, const GLenum type);
  GLuint GPU_vertex_format_stride(const int id);
  GLuint GPU_vertex_format_offset(const int id, const GLuint idx);
  GLuint GPU_vertex_format_attribute_count(const int id);
  void GPU_vertex_format_add_4(const int id, const GLuint attribute_id,
    GLvoid *data, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4);
  void GPU_vertex_format_bind(const int id, GLvoid *data);
#endif
