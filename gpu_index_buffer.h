// gpu_index_buffer.h

#ifndef _GPU_INDEX_BUFFER_H_
  #define _GPU_INDEX_BUFFER_H_

  typedef struct IndexBuffer IndexBuffer;

  IndexBuffer *GPU_indexbuf_create(void);
  void GPU_indexbuf_delete(IndexBuffer *ibuff);
  void GPU_indexbuf_begin_update(IndexBuffer *ibuff, const GLuint max_count);
  void GPU_indexbuf_add_3(IndexBuffer *ibuff, const GLshort val1, const GLshort val2, const GLshort val3);
  void GPU_indexbuf_use_VBO(IndexBuffer *ibuff);
  void GPU_indexbuf_bind(IndexBuffer *ibuff);
  GLvoid *GPU_indexbuf_get_index(IndexBuffer *ibuff);

#endif
