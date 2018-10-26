// shaders.h
#ifndef _SHADERS_H_
  #define _SHADERS_H_

#include "shaderid.h"

void enable_shader_program(PROGRAM_ID_T programID);
GLint get_active_uniform_location(const char *name);
GLint get_active_attribute_location(const char *name);
void load_shader_programs(void);

#endif
