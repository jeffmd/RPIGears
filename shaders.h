// shaders.h
#ifndef _SHADERS_H_
  #define _SHADERS_H_

typedef enum {
  BLINN_PHONG_PRG  
} PROGRAM_ID_T;

void enable_shader_program(PROGRAM_ID_T programID);
GLuint shader_UBO_location(void);
GLuint shader_MaterialColor_location(void);
GLuint shader_DiffuseMap_location(void);
void load_shader_programs(void);

#endif
