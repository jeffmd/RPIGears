// gpu_vertex_format.h

#ifndef _GPU_VERTEX_FORMAT_H_
  #define _GPU_VERTEX_FORMAT_H_

  ID_t GPU_vertex_format_create(void);
  void GPU_vertex_format_delete(const ID_t id);
  void GPU_vertex_format_add_attribute(const ID_t id, const char *name,
    const GLint size, const GLenum type);
  GLuint GPU_vertex_format_stride(const ID_t id);
  GLuint GPU_vertex_format_offset(const ID_t id, const GLuint idx);
  GLuint GPU_vertex_format_attribute_count(const ID_t id);
  void GPU_vertex_format_add_4(const ID_t id, const GLuint attribute_id,
    GLvoid *data, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4);
  void GPU_vertex_format_read_data(const ID_t id, const GLuint attribute_id, GLvoid *attr_data, GLfloat val[4]);
  void GPU_vertex_format_bind(const ID_t id, GLvoid *data);
#endif
