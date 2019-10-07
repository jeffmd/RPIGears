// gpu_uniform_buffer.h

#ifndef _GPU_UNIFORM_BUFFER_H_
  #define _GPU_UNIFORM_BUFFER_H_
  

  int GPU_uniformbuffer_create(void);
  void GPU_uniformbuffer_delete(const int id);
  void GPU_uniformbuffer_add(const int id, const char *name,
    const GLint size, const GLenum type, void *data);
  void GPU_uniformbuffer_bind(const int id);
  void GPU_uniformbuffer_update(const int id);
  void GPU_uniformbuffer_update_active(void);
  void GPU_uniformbuffer_activate(const int id);

#define GPU_uniformbuffer_add_4f(ubuff, name, data) GPU_uniformbuffer_add(ubuff, name, sizeof(data)/(sizeof(float)*4), GL_FLOAT_VEC4, &data)
#define GPU_uniformbuffer_add_1f(ubuff, name, data) GPU_uniformbuffer_add(ubuff, name, sizeof(data)/sizeof(float), GL_FLOAT, &data)
#define GPU_uniformbuffer_add_1i(ubuff, name, data) GPU_uniformbuffer_add(ubuff, name, sizeof(data)/sizeof(int), GL_INT, &data)
#define GPU_uniformbuffer_add_4x4m(ubuff, name, data) GPU_uniformbuffer_add(ubuff, name, sizeof(data)/(sizeof(float)*4*4), GL_FLOAT_MAT4, &data)

#endif
