// gpu_batch.h

#ifndef _GPU_BATCH_H_
  #define _GPU_BATCH_H_

  short GPU_batch_create(void);
  void GPU_batch_delete(const short id, const int delete_buffers);
  void GPU_batch_set_indices_draw_count(const short id, const int count);
  void GPU_batch_set_vertices_draw_count(const short id, const int count);
  void GPU_batch_set_draw_start(const short id, uint16_t start);
  void GPU_batch_draw(const short id, const GLenum drawMode, const GLuint instances);
  void GPU_batch_set_vertex_buffer(const short id, const short vbuff);
  void GPU_batch_set_index_buffer(const short id, const short ibuff);
  void GPU_batch_set_uniform_buffer(const short id, const short ubuff);
  short GPU_batch_vertex_buffer(const short id);
  short GPU_batch_index_buffer(const short id);
  short GPU_batch_uniform_buffer(const short id);
  void GPU_batch_use_BO(const short id);
  void GPU_batch_no_BO(const short id);

  short GPU_batch_part_create(const short batch);
  void GPU_batch_part_draw(const short part, const GLenum drawMode, const GLuint instances);
  void GPU_batch_part_end(const short part);

#endif
