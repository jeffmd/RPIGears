// gpu_shader.h

#ifndef _GPU_SHADER_H_
#define _GPU_SHADER_H_

  typedef struct GPUShader GPUShader;

  GPUShader *GPU_shader_create(const char *vertex_file_name, const char *fragment_file_name);
  void GPU_shader_init(GPUShader *shader);
  void GPU_shader_gldelete(GPUShader *shader);

	GLuint GPU_shader_glprogram_obj(GPUShader *shader);
	void GPU_shader_bind(GPUShader *shader);
	GLint GPU_shader_uniform_location(GPUShader *shader, const char *name);
	GLint GPU_shader_attribute_location(GPUShader *shader, const char *name);

	GLint GPU_get_active_uniform_location(const char *name);
	GLint GPU_get_active_attribute_location(const char *name);

#endif
