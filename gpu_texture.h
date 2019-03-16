// gpu_texture.h
#ifndef _GPU_TEXTURE_H_
#define _GPU_TEXTURE_H_

typedef enum {
	/* Formats texture & renderbuffer */
	GPU_TF_NONE = 0,
	GPU_R8,
	GPU_RG8,
	GPU_RGB8,
	GPU_RGBA8,
	GPU_STENCIL8,
	GPU_DEPTH16,
	GPU_DEPTH24,
	GPU_DEPTH32
} GPUTextureFormat;

typedef struct GPUTexture GPUTexture;

GPUTexture *GPU_texture_create_2D(const int w, const int h,
        const GPUTextureFormat tex_format, const void *pixels);

void GPU_texture_bind(GPUTexture *tex, const int slot);
void GPU_texture_unbind(GPUTexture *tex);
void GPU_texture_free(GPUTexture *tex);
int GPU_texture_bound_slot(GPUTexture *tex);
void GPU_texture_ref(GPUTexture *tex);
int GPU_texture_target(GPUTexture *tex);
int GPU_texture_width(GPUTexture *tex);
int GPU_texture_height(GPUTexture *tex);
GPUTextureFormat GPU_texture_format(GPUTexture *tex);
GLboolean GPU_texture_cube(GPUTexture *tex);
GLuint GPU_texture_opengl_bindcode(GPUTexture *tex);


#endif // _GPU_TEXTURE_H_
