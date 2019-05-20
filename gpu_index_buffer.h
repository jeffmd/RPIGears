// gpu_index_buffer.h

#ifndef _GPU_INDEX_BUFFER_H_
  #define _GPU_INDEX_BUFFER_H_

  typedef struct GPUIndexBuffer GPUIndexBuffer;

  GPUIndexBuffer *GPU_indexbuf_create(void);
  void GPU_indexbuf_delete(GPUIndexBuffer *ibuff);
  void GPU_indexbuf_begin_update(GPUIndexBuffer *ibuff, const GLuint max_count);
  void GPU_indexbuf_add_3(GPUIndexBuffer *ibuff, const GLshort val1, const GLshort val2, const GLshort val3);
  void GPU_indexbuf_use_BO(GPUIndexBuffer *ibuff);
  void GPU_indexbuf_no_BO(GPUIndexBuffer *ibuff);
  void GPU_indexbuf_bind(GPUIndexBuffer *ibuff);
  GLvoid *GPU_indexbuf_get_index(GPUIndexBuffer *ibuff);

#endif
