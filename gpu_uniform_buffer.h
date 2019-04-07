// gpu_uniform_buffer.h

#ifndef _GPU_UNIFORM_BUFFER_H_
  #define _GPU_UNIFORM_BUFFER_H_
  
  typedef struct GPUUniformBuffer GPUUniformBuffer;

  GPUUniformBuffer *GPU_uniformbuffer_create(void);
  void GPU_uniformbuffer_delete(GPUUniformBuffer *ubuff);
  void GPU_uniformbuffer_add_uniform(GPUUniformBuffer *ubuff, const char *name,
    const GLint size, const GLenum type, void *data);
  void GPU_uniformbuffer_bind(GPUUniformBuffer *ubuff);
  void GPU_uniformbuffer_update(GPUUniformBuffer *ubuff);

#endif
