// gpu_shader_unit.h

#ifndef _GPU_SHADER_UNIT_H_
  #define _GPU_SHADER_UNIT_H_

  ID_t GPU_shader_unit(const char *file_name, const GLuint type);
  GLuint GPU_shader_unit_globj(const ID_t id);
  void GPU_shader_unit_reset(const ID_t id);
  uint8_t GPU_shader_unit_modid(const ID_t id);
  const char *GPU_shader_unit_file_name(const ID_t id);

#endif
