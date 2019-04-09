// gpu_batch.h

#ifndef _GPU_BATCH_H_
  #define _GPU_BATCH_H_

  typedef struct GPUBatch GPUBatch;

  void GPU_batch_init(GPUBatch *batch);
  GPUBatch *GPU_batch_create(void);
  void GPU_batch_delete(GPUBatch *batch, const int delete_buffers);
  void GPU_batch_set_indices_draw_count(GPUBatch *batch, const int count);
  void GPU_batch_set_vertices_draw_count(GPUBatch *batch, const int count);
  void GPU_batch_draw(GPUBatch *batch, const GLenum drawMode, const GLuint instances);
  void GPU_batch_set_vertex_buffer(GPUBatch *batch, GPUVertBuffer *vbuff);
  void GPU_batch_set_index_buffer(GPUBatch *batch, GPUIndexBuffer *ibuff);
  void GPU_batch_set_uniform_buffer(GPUBatch *batch, GPUUniformBuffer *ubuff);
  GPUVertBuffer *GPU_batch_vertex_buffer(GPUBatch *batch);
  GPUIndexBuffer *GPU_batch_index_buffer(GPUBatch *batch);
  GPUUniformBuffer *GPU_batch_uniform_buffer(GPUBatch *batch);
  
#endif
