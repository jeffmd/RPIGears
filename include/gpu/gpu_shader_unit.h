// gpu_shader_unit.h

#ifndef _GPU_SHADER_UNIT_H_
  #define _GPU_SHADER_UNIT_H_

  short GPU_shader_unit(const char *file_name, const GLuint type);
  GLuint GPU_shader_unit_globj(const short id);
  void GPU_shader_unit_reset(const short id);
  uint8_t GPU_shader_unit_modid(const short id);
  const char *GPU_shader_unit_file_name(const short id);

#endif
