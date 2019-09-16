// gpu_quad.h

#ifndef _GPU_QUAD_H_
#define _GPU_QUAD_H_

  int GPU_quad_create(void);
  void GPU_quad_delete(const int id);
  void GPU_quad_draw(const int id);
  void GPU_quad_set_texture(const int id, const int tex_id);
  void GPU_quad_set_shader(const int id, const int shader_id);

#endif
