// gpu_index_buffer.h

#ifndef _GPU_INDEX_BUFFER_H_
  #define _GPU_INDEX_BUFFER_H_

  short GPU_indexbuf_create(void);
  void GPU_indexbuf_delete(const short id);
  void GPU_indexbuf_set_add_count(const short id, const GLuint max_count);
  void GPU_indexbuf_add(const short id, const GLshort val);
  void GPU_indexbuf_use_BO(const short id);
  void GPU_indexbuf_no_BO(const short id);
  void GPU_indexbuf_bind(const short id);
  GLvoid *GPU_indexbuf_start(const short id);
  GLuint GPU_indexbuf_index(const short id);
  void GPU_indexbuf_set_index(const short id, const GLuint start);

#endif
