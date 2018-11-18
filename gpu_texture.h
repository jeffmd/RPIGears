// gpu_texture.h
#ifndef _GPU_TEXTURE_H_
#define _GPU_TEXTURE_H_

typedef enum GPUTextureFormat {
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

GLuint GPU_texture_create_2D(const int w, const int h,
        const GPUTextureFormat tex_format, const void *pixels);

void GPU_texture_bind(const GLuint texID, const int slot);
void GPU_texture_unbind(const GLuint texID);
void GPU_texture_free(const GLuint texID);
int GPU_texture_bound_slot(const GLuint texID);
void GPU_texture_ref(const GLuint texID);
int GPU_texture_target(const GLuint texID);
int GPU_texture_width(const GLuint texID);
int GPU_texture_height(const GLuint texID);
GPUTextureFormat GPU_texture_format(const GLuint texID);
GLboolean GPU_texture_cube(const GLuint texID);
GLuint GPU_texture_opengl_bindcode(const GLuint texID);


#endif // _GPU_TEXTURE_H_
