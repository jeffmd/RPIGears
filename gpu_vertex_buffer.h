// gpu_vertex_buffer.h

#ifndef _GPU_VERTEX_BUFFER_H_
  #define _GPU_VERTEX_BUFFER_H_

  int GPU_vertbuf_create(void);
  void GPU_vertbuf_delete(int id);
  void GPU_vertbuf_set_start(int id, const GLuint index);
  void GPU_vertbuf_set_add_count(int id, const GLuint count);
  void GPU_vertbuf_set_vertex_format(int id, GPUVertFormat *vformat);
  void GPU_vertbuf_add_4(int id, const GLuint attribute_id, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4);
  void GPU_vertbuf_use_BO(int id);
  void GPU_vertbuf_no_BO(int id);
  void GPU_vertbuf_bind(int id);

  #define GPU_vertbuf_add_3(vbuff, attr_id, val1, val2, val3) GPU_vertbuf_add_4(vbuff, attr_id, val1, val2, val3, 0)
  #define GPU_vertbuf_add_2(vbuff, attr_id, val1, val2) GPU_vertbuf_add_4(vbuff, attr_id, val1, val2, 0, 0)

#endif
