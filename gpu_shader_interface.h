// gpu_shader_interface.h

#ifndef _GPU_SHADER_INTERFACE_H_
#define _GPU_SHADER_INTERFACE_H_

#include "shaderid.h"

GLuint get_shader_program_obj(PROGRAM_ID_T programID);
GLuint init_shader_interface(const PROGRAM_ID_T programID);
void enable_shader_program(PROGRAM_ID_T programID);
GLint get_active_uniform_location(const char *name);
GLint get_active_attribute_location(const char *name);

#endif
