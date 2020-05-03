// gpu_shader.h

#ifndef _GPU_SHADER_H_
#define _GPU_SHADER_H_

  short GPU_shader(const char *vertex_file_name, const char *fragment_file_name);
  void GPU_shader_reset(const short id);

  short GPU_shader_active_shader(void);
  uint8_t GPU_shader_modid(const short id);
  void GPU_shader_bind(const short id);
  GLint GPU_shader_uniform_location(const short id, const char *name);
  GLint GPU_shader_attribute_location(const short id, const char *name);

  GLint GPU_get_active_uniform_location(const char *name);
  GLint GPU_get_active_attribute_location(const char *name);

#endif
