// gpu_framebuffer.h
#ifndef __GPU_FRAMEBUFFER_H__
#define __GPU_FRAMEBUFFER_H__

typedef enum GPUFrameBufferBits {
	GPU_COLOR_BIT    = GL_COLOR_BUFFER_BIT,
	GPU_DEPTH_BIT    = GL_DEPTH_BUFFER_BIT,
	GPU_STENCIL_BIT  = GL_STENCIL_BUFFER_BIT
} GPUFrameBufferBits;

ID_t GPU_framebuffer_create(void);
void GPU_framebuffer_free(const ID_t id);
void GPU_framebuffer_texture_detach(const ID_t id, const ID_t tex);
void GPU_framebuffer_texture_detach_all(const ID_t tex);
void GPU_framebuffer_texture_attach(const ID_t id, const ID_t tex);
void GPU_framebuffer_bind(const ID_t id);
void GPU_framebuffer_done(void);

#endif  /* __GPU_FRAMEBUFFER_H__ */
