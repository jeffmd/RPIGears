// gpu_vertex_buffer.h

#ifndef _GPU_VERTEX_BUFFER_H_
  #define _GPU_VERTEX_BUFFER_H_

  typedef struct GPUVertBuffer GPUVertBuffer;

  GPUVertBuffer *GPU_vertbuf_create(void);
  void GPU_vertbuf_delete(GPUVertBuffer *vbuff);
  void GPU_vertbuf_begin_update(GPUVertBuffer *vbuff, const GLuint max_count);
  void GPU_vertbuf_set_vertex_format(GPUVertBuffer *vbuff, GPUVertFormat *vformat);
  void GPU_vertbuf_add_4(GPUVertBuffer *vbuff, const GLuint attribute_id, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4);
  void GPU_vertbuf_use_VBO(GPUVertBuffer *vbuff);
  void GPU_vertbuf_bind(GPUVertBuffer *vbuff_id);

  #define GPU_vertbuf_add_3(vbuff, attr_id, val1, val2, val3) GPU_vertbuf_add_4(vbuff, attr_id, val1, val2, val3, 0)
  #define GPU_vertbuf_add_2(vbuff, attr_id, val1, val2) GPU_vertbuf_add_4(vbuff, attr_id, val1, val2, 0, 0)

#endif
