// gpu_index_buffer.h

#ifndef _GPU_INDEX_BUFFER_H_
  #define _GPU_INDEX_BUFFER_H_

  ID_t GPU_indexbuf_create(void);
  void GPU_indexbuf_delete(const ID_t id);
  void GPU_indexbuf_set_add_count(const ID_t id, const GLuint max_count);
  void GPU_indexbuf_add(const ID_t id, const GLshort val);
  void GPU_indexbuf_use_BO(const ID_t id);
  void GPU_indexbuf_no_BO(const ID_t id);
  void GPU_indexbuf_bind(const ID_t id);
  GLvoid *GPU_indexbuf_start(const ID_t id);
  GLuint GPU_indexbuf_index(const ID_t id);
  void GPU_indexbuf_set_index(const ID_t id, const GLuint start);

#endif
