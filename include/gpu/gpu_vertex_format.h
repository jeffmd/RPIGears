// gpu_vertex_format.h

#ifndef _GPU_VERTEX_FORMAT_H_
  #define _GPU_VERTEX_FORMAT_H_

  short GPU_vertex_format_create(void);
  void GPU_vertex_format_delete(const short id);
  void GPU_vertex_format_add_attribute(const short id, const char *name,
    const GLint size, const GLenum type);
  GLuint GPU_vertex_format_stride(const short id);
  GLuint GPU_vertex_format_offset(const short id, const GLuint idx);
  GLuint GPU_vertex_format_attribute_count(const short id);
  void GPU_vertex_format_add_4(const short id, const GLuint attribute_id,
    GLvoid *data, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4);
  void GPU_vertex_format_read_data(const short id, const GLuint attribute_id, GLvoid *attr_data, GLfloat val[4]);
  void GPU_vertex_format_bind(const short id, GLvoid *data);
#endif
