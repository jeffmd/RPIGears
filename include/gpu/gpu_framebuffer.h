// gpu_framebuffer.h
#ifndef __GPU_FRAMEBUFFER_H__
#define __GPU_FRAMEBUFFER_H__

typedef enum GPUFrameBufferBits {
	GPU_COLOR_BIT    = GL_COLOR_BUFFER_BIT,
	GPU_DEPTH_BIT    = GL_DEPTH_BUFFER_BIT,
	GPU_STENCIL_BIT  = GL_STENCIL_BUFFER_BIT
} GPUFrameBufferBits;

int GPU_framebuffer_create(void);
void GPU_framebuffer_free(const int id);
void GPU_framebuffer_texture_detach(const int id, const int tex);
void GPU_framebuffer_texture_detach_all(const int tex);
void GPU_framebuffer_texture_attach(const int id, const int tex);
void GPU_framebuffer_bind(const int id);
void GPU_framebuffer_done(void);

#endif  /* __GPU_FRAMEBUFFER_H__ */
