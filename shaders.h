// shaders.h
#ifndef _SHADERS_H_
  #define _SHADERS_H_

#include "shaderid.h"

void enable_shader_program(PROGRAM_ID_T programID);
GLuint get_uniform_location(const UNIFORM_ID_T id);
void load_shader_programs(void);

#endif
