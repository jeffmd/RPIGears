// gpu_uniform_buffer.h

  GLuint GPU_uniformbuffer_create(void);
  void GPU_uniformbuffer_delete(const GLuint vbuff_id);
  void GPU_uniformbuffer_add_uniform(const GLuint ubuff_id, const char *name,
    const GLint size, const GLenum type);
