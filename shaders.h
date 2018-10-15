// shaders.h
#ifndef _SHADERS_H_
  #define _SHADERS_H_

void enable_shader_program();
void init_shader_system(void);
void update_uniform_locations(void);
GLuint shader_UBO_location(void);
GLuint shader_MaterialColor_location(void);
GLuint shader_DiffuseMap_location(void);
void load_shader_program(void);

#endif
