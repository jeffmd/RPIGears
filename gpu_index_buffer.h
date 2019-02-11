// gpu_index_buffer.h

#ifndef _GPU_INDEX_BUFFER_H_
  #define _GPU_INDEX_BUFFER_H_

  GLuint GPU_indexbuf_create(void);
  void GPU_indexbuf_delete(const GLuint vbuff_id);
  void GPU_indexbuf_begin_update(const GLuint vbuff_id, const GLuint max_count);
  void GPU_indexbuf_add_3(const GLuint ibuff_id, const GLshort val1, const GLshort val2, const GLshort val3);
  void GPU_indexbuf_use_VBO(const GLuint vbuff_id);
  void GPU_indexbuf_set_VAO(const GLuint vbuff_id);
  GLvoid *GPU_indexbuf_get_index(const GLuint ibuff_id);

#endif
