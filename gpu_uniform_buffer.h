// gpu_uniform_buffer.h

#ifndef _GPU_UNIFORM_BUFFER_H_
  #define _GPU_UNIFORM_BUFFER_H_
  
  typedef struct UniformBuffer UniformBuffer;

  UniformBuffer *GPU_uniformbuffer_create(void);
  void GPU_uniformbuffer_delete(UniformBuffer *ubuff);
  void GPU_uniformbuffer_add_uniform(UniformBuffer *ubuff, const char *name,
    const GLint size, const GLenum type);

#endif
