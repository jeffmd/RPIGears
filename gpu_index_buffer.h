// gpu_index_buffer.h

#ifndef _GPU_INDEX_BUFFER_H_
  #define _GPU_INDEX_BUFFER_H_

  int GPU_indexbuf_create(void);
  void GPU_indexbuf_delete(int id);
  void GPU_indexbuf_set_add_count(int id, const GLuint max_count);
  void GPU_indexbuf_add(int id, const GLshort val);
  void GPU_indexbuf_use_BO(int id);
  void GPU_indexbuf_no_BO(int id);
  void GPU_indexbuf_bind(int id);
  GLvoid *GPU_indexbuf_get_index(int id);

#endif
