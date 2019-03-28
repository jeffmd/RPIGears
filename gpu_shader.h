// gpu_shader.h

#ifndef _GPU_SHADER_H_
#define _GPU_SHADER_H_

  typedef struct GPUShader GPUShader;

  GPUShader *GPU_shader_create(const char *vertex_file_name, const char *fragment_file_name);
  void GPU_shader_init(GPUShader *interface);
  void GPU_shader_gldelete(GPUShader *interface);

	GLuint GPU_shader_glprogram_obj(GPUShader *interface);
	void GPU_shader_bind(GPUShader *interface);
  
	GLint GPU_get_active_uniform_location(const char *name);
	GLint GPU_get_active_attribute_location(const char *name);

#endif
