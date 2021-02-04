// gpu_quad.h

#ifndef _GPU_QUAD_H_
#define _GPU_QUAD_H_

  ID_t GPU_quad_create(void);
  void GPU_quad_delete(const ID_t id);
  void GPU_quad_draw(const ID_t id);
  void GPU_quad_set_texture(const ID_t id, const ID_t tex_id);
  void GPU_quad_set_shader(const ID_t id, const ID_t shader_id);
  void GPU_quad_set_alimit(const ID_t id, const float val);

#endif
