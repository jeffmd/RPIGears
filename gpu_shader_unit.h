// gpu_shader_unit.h

#ifndef _GPU_SHADER_UNIT_H_
  #define _GPU_SHADER_UNIT_H_

  int GPU_shader_unit(const char *file_name, const GLuint type);
  GLuint GPU_shader_unit_globj(const int id);
  void GPU_shader_unit_reset(const int id);
  int GPU_shader_unit_modid(const int id);
  const char *GPU_shader_unit_file_name(const int id);

#endif
