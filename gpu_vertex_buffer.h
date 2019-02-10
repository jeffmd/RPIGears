// gpu_vertex_buffer.h

#ifndef _GPU_VERTEX_BUFFER_H_
  #define _GPU_VERTEX_BUFFER_H_

  GLuint GPU_vertbuf_create(void);
  void GPU_vertbuf_delete(const GLuint vbuff_id);
  void GPU_vertbuf_add_attribute(const GLuint vbuff_id, const char *name,
    const GLint size, const GLenum type);
  void GPU_vertbuf_begin_update(const GLuint vbuff_id, const GLuint max_count);
  void GPU_vertbuf_add_vertex_3f(const GLuint attribute_id, const GLfloat val1, const GLfloat val2, const GLfloat val3);

#endif
