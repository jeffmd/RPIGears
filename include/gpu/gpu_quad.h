// gpu_quad.h

#ifndef _GPU_QUAD_H_
#define _GPU_QUAD_H_

  short GPU_quad_create(void);
  void GPU_quad_delete(const short id);
  void GPU_quad_draw(const short id);
  void GPU_quad_set_texture(const short id, const short tex_id);
  void GPU_quad_set_shader(const short id, const short shader_id);
  void GPU_quad_set_alimit(const short id, const float val);

#endif
