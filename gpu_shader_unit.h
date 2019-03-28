// gpu_shader_unit.h

#ifndef _GPU_SHADER_UNIT_H_
  #define _GPU_SHADER_UNIT_H_
  
  typedef struct GPUShaderUnit GPUShaderUnit;

  GPUShaderUnit *GPU_shader_unit_create(const char *file_name, const GLuint type);
  void GPU_shader_unit_init(GPUShaderUnit *shader);
  GLuint GPU_shader_unit_globj(GPUShaderUnit *shader);
  void GPU_shader_unit_glDelete(GPUShaderUnit *shader);

#endif
