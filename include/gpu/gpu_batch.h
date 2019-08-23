// gpu_batch.h

#ifndef _GPU_BATCH_H_
  #define _GPU_BATCH_H_

  int GPU_batch_create(void);
  void GPU_batch_delete(const int id, const int delete_buffers);
  void GPU_batch_set_indices_draw_count(const int id, const int count);
  void GPU_batch_set_vertices_draw_count(const int id, const int count);
  void GPU_batch_draw(const int id, const GLenum drawMode, const GLuint instances);
  void GPU_batch_set_vertex_buffer(const int id, const int vbuff);
  void GPU_batch_set_index_buffer(const int id, const int ibuff);
  void GPU_batch_set_uniform_buffer(const int id, const int ubuff);
  int GPU_batch_vertex_buffer(const int id);
  int GPU_batch_index_buffer(const int id);
  int GPU_batch_uniform_buffer(const int id);
  void GPU_batch_use_BO(const int id);
  void GPU_batch_no_BO(const int id);

  
#endif
