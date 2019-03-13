// gpu_vertex_format.h

#ifndef _GPU_VERTEX_FORMAT_H_
  #define _GPU_VERTEX_FORMAT_H_

  
  void GPU_vertex_format_init(const GLuint vformat_id);
  GLuint GPU_vertex_format_create(void);
  void GPU_vertex_format_delete(const GLuint vformat_id);
  void GPU_vertex_format_add_attribute(const GLuint vformat_id, const char *name,
    const GLint size, const GLenum type);
  GLuint GPU_vertex_format_stride(const GLuint vformat_id);
  GLuint GPU_vertex_format_offset(const GLuint vformat_id, const GLuint idx);
  GLuint GPU_vertex_format_attribute_count(const GLuint vformat_id);
  void GPU_vertex_format_add_4(const GLuint vformat_id, const GLuint attribute_id,
    GLvoid *data, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4);
  void GPU_vertex_format_bind(const GLuint vformat_id, GLvoid *data);
#endif
