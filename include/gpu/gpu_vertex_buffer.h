// gpu_vertex_buffer.h

#ifndef _GPU_VERTEX_BUFFER_H_
  #define _GPU_VERTEX_BUFFER_H_

  short GPU_vertbuf_create(void);
  void GPU_vertbuf_delete(const short id);
  void GPU_vertbuf_set_index(const short id, const GLuint index);
  GLuint GPU_vertbuf_index(const short id);
  void GPU_vertbuf_set_add_count(const short id, const GLuint count);
  void GPU_vertbuf_set_vertex_format(const short id, const int vformat);
  void GPU_vertbuf_add_4(const short id, const GLuint attribute_id, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4);
  void GPU_vertbuf_read_data(const short id, const GLuint attribute_id, GLfloat val[4]);
  void GPU_vertbuf_use_BO(const short id);
  void GPU_vertbuf_no_BO(const short id);
  void GPU_vertbuf_bind(const short id);

  #define GPU_vertbuf_add_3(vbuff, attr_id, val1, val2, val3) GPU_vertbuf_add_4(vbuff, attr_id, val1, val2, val3, 0)
  #define GPU_vertbuf_add_2(vbuff, attr_id, val1, val2) GPU_vertbuf_add_4(vbuff, attr_id, val1, val2, 0, 0)

#endif
