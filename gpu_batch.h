// gpu_batch.h

#ifndef _GPU_BATCH_H_
  #define _GPU_BATCH_H_

  int GPU_batch_create(void);
  void GPU_batch_delete(int id, const int delete_buffers);
  void GPU_batch_set_indices_draw_count(int id, const int count);
  void GPU_batch_set_vertices_draw_count(int id, const int count);
  void GPU_batch_draw(int id, const GLenum drawMode, const GLuint instances);
  void GPU_batch_set_vertex_buffer(int id, GPUVertBuffer *vbuff);
  void GPU_batch_set_index_buffer(int id, GPUIndexBuffer *ibuff);
  void GPU_batch_set_uniform_buffer(int id, int ubuff);
  GPUVertBuffer *GPU_batch_vertex_buffer(int id);
  GPUIndexBuffer *GPU_batch_index_buffer(int id);
  int GPU_batch_uniform_buffer(int id);
  void GPU_batch_use_BO(int id);
  void GPU_batch_no_BO(int id);

  
#endif
