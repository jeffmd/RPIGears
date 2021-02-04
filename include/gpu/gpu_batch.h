// gpu_batch.h

#ifndef _GPU_BATCH_H_
  #define _GPU_BATCH_H_

  ID_t GPU_batch_create(void);
  void GPU_batch_delete(const ID_t id, const int delete_buffers);
  void GPU_batch_set_indices_draw_count(const ID_t id, const int count);
  void GPU_batch_set_vertices_draw_count(const ID_t id, const int count);
  void GPU_batch_set_draw_start(const ID_t id, uint16_t start);
  void GPU_batch_draw(const ID_t id, const GLenum drawMode, const GLuint instances);
  void GPU_batch_set_vertex_buffer(const ID_t id, const short vbuff);
  void GPU_batch_set_index_buffer(const ID_t id, const short ibuff);
  void GPU_batch_set_uniform_buffer(const ID_t id, const short ubuff);
  ID_t GPU_batch_vertex_buffer(const ID_t id);
  ID_t GPU_batch_index_buffer(const ID_t id);
  ID_t GPU_batch_uniform_buffer(const ID_t id);
  void GPU_batch_use_BO(const ID_t id);
  void GPU_batch_no_BO(const ID_t id);

  ID_t GPU_batch_part_create(const ID_t batch);
  void GPU_batch_part_draw(const ID_t part, const GLenum drawMode, const GLuint instances);
  void GPU_batch_part_end(const ID_t part);

#endif
