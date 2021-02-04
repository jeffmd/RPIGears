// gpu_shader.h

#ifndef _GPU_SHADER_H_
#define _GPU_SHADER_H_

  ID_t GPU_shader(const char *vertex_file_name, const char *fragment_file_name);
  void GPU_shader_reset(const ID_t id);

  ID_t GPU_shader_active_shader(void);
  uint8_t GPU_shader_modid(const ID_t id);
  void GPU_shader_bind(const ID_t id);
  GLint GPU_shader_uniform_location(const ID_t id, const char *name);
  GLint GPU_shader_attribute_location(const ID_t id, const char *name);

  GLint GPU_get_active_uniform_location(const char *name);
  GLint GPU_get_active_attribute_location(const char *name);

#endif
