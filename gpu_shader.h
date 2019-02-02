// gpu_shader.h

#ifndef _GPU_SHADER_H_
  #define _GPU_SHADER_H_
  
  #include "gles3.h"
  
  GLuint shader_program_create(const char *vertex_file_name, const char *fragment_file_name);
  void init_shader_program(const GLuint programID);
  void reset_shader_program(const GLuint programID);

#endif
