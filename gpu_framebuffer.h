// gpu_framebuffer.h
#ifndef __GPU_FRAMEBUFFER_H__
#define __GPU_FRAMEBUFFER_H__

typedef enum GPUFrameBufferBits {
	GPU_COLOR_BIT    = GL_COLOR_BUFFER_BIT,
	GPU_DEPTH_BIT    = GL_DEPTH_BUFFER_BIT,
	GPU_STENCIL_BIT  = GL_STENCIL_BUFFER_BIT
} GPUFrameBufferBits;

GLuint GPU_framebuffer_create(void);
void GPU_framebuffer_free(GLuint framebufferID);
void GPU_framebuffer_texture_detach(GLuint fbID, GLuint texID);
void GPU_framebuffer_texture_detach_all(GLuint texID);
void GPU_framebuffer_texture_attach(GLuint fbID, GLuint texID);
void GPU_framebuffer_bind(GLuint fbID);

#endif  /* __GPU_FRAMEBUFFER_H__ */
