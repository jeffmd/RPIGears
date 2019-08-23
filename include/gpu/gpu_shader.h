// gpu_shader.h

#ifndef _GPU_SHADER_H_
#define _GPU_SHADER_H_

  int GPU_shader(const char *vertex_file_name, const char *fragment_file_name);
  void GPU_shader_reset(const int id);

  int GPU_shader_active_shader(void);
  int GPU_shader_modid(const int id);
  void GPU_shader_bind(const int id);
  GLint GPU_shader_uniform_location(const int id, const char *name);
  GLint GPU_shader_attribute_location(const int id, const char *name);

  GLint GPU_get_active_uniform_location(const char *name);
  GLint GPU_get_active_attribute_location(const char *name);

#endif