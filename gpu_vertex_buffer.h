// gpu_vertex_buffer.h

#ifndef _GPU_VERTEX_BUFFER_H_
  #define _GPU_VERTEX_BUFFER_H_

  GLuint GPU_vertbuf_create(void);
  void GPU_vertbuf_delete(const GLuint vbuff_id);
  void GPU_vertbuf_begin_update(const GLuint vbuff_id, const GLuint max_count);
  void GPU_vertbuf_set_vertex_format(const GLuint vbuff_id, VertFormat *vformat);
  void GPU_vertbuf_add_4(const GLuint vbuff_id, const GLuint attribute_id, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4);
  void GPU_vertbuf_use_VBO(const GLuint vbuff_id);
  void GPU_vertbuf_bind(const GLuint vbuff_id);

  #define GPU_vertbuf_add_3(vbuff_id, attr_id, val1, val2, val3) GPU_vertbuf_add_4(vbuff_id, attr_id, val1, val2, val3, 0)
  #define GPU_vertbuf_add_2(vbuff_id, attr_id, val1, val2) GPU_vertbuf_add_4(vbuff_id, attr_id, val1, val2, 0, 0)

#endif
