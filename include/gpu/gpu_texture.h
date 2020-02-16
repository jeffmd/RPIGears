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
	GPU_DEPTH32,
        GPU_VCSM      // videocore shared memory
} GPUTextureFormat;

int GPU_texture_create(const int w, const int h,
        const GPUTextureFormat tex_format, const void *pixels);

void GPU_texture_bind(int id, const int slot);
void GPU_texture_unbind(int id);
void GPU_texture_free(int id);
int GPU_texture_bound_slot(int id);
void GPU_texture_ref(int id);
int GPU_texture_target(int id);
int GPU_texture_width(int id);
int GPU_texture_height(int id);
GPUTextureFormat GPU_texture_format(int id);
GLboolean GPU_texture_cube(int id);
GLuint GPU_texture_opengl_bindcode(int id);
void GPU_texture_sub_image(int id, const GLint xoffset,
  const GLint yoffset, const GLsizei width,const GLsizei height, const void *pixels);
void GPU_texture_mipmap(int id);
unsigned char *GPU_texture_vcsm_lock(void);
void GPU_texture_vcsm_unlock(void);

#endif // _GPU_TEXTURE_H_
