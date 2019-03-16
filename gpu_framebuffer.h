// gpu_framebuffer.h
#ifndef __GPU_FRAMEBUFFER_H__
#define __GPU_FRAMEBUFFER_H__

typedef struct GPUFrameBuffer GPUFrameBuffer;

typedef enum GPUFrameBufferBits {
	GPU_COLOR_BIT    = GL_COLOR_BUFFER_BIT,
	GPU_DEPTH_BIT    = GL_DEPTH_BUFFER_BIT,
	GPU_STENCIL_BIT  = GL_STENCIL_BUFFER_BIT
} GPUFrameBufferBits;

GPUFrameBuffer *GPU_framebuffer_create(void);
void GPU_framebuffer_free(GPUFrameBuffer *fb);
void GPU_framebuffer_texture_detach(GPUFrameBuffer *fb, GPUTexture *tex);
void GPU_framebuffer_texture_detach_all(GPUTexture *tex);
void GPU_framebuffer_texture_attach(GPUFrameBuffer *fb, GPUTexture *tex);
void GPU_framebuffer_bind(GPUFrameBuffer *fb);

#endif  /* __GPU_FRAMEBUFFER_H__ */
