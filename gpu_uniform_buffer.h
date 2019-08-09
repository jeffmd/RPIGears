// gpu_uniform_buffer.h

#ifndef _GPU_UNIFORM_BUFFER_H_
  #define _GPU_UNIFORM_BUFFER_H_
  

  int GPU_uniformbuffer_create(void);
  void GPU_uniformbuffer_delete(int id);
  void GPU_uniformbuffer_add_uniform(int id, const char *name,
    const GLint size, const GLenum type, void *data);
  void GPU_uniformbuffer_bind(int id);
  void GPU_uniformbuffer_update(int id);
  void GPU_uniformbuffer_update_active(void);
  void GPU_uniformbuffer_activate(int id);

#define GPU_uniformbuffer_add_uniform_4f(ubuff, name, data) GPU_uniformbuffer_add_uniform(ubuff, name, sizeof(data)/(sizeof(float)*4), GL_FLOAT_VEC4, &data)
#define GPU_uniformbuffer_add_uniform_1f(ubuff, name, data) GPU_uniformbuffer_add_uniform(ubuff, name, sizeof(data)/sizeof(float), GL_FLOAT, &data)
#define GPU_uniformbuffer_add_uniform_1i(ubuff, name, data) GPU_uniformbuffer_add_uniform(ubuff, name, sizeof(data)/sizeof(int), GL_INT, &data)
#define GPU_uniformbuffer_add_uniform_4x4m(ubuff, name, data) GPU_uniformbuffer_add_uniform(ubuff, name, sizeof(data)/(sizeof(float)*4*4), GL_FLOAT_MAT4, &data)

#endif
