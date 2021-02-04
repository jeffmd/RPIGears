// gpu_texture.c

#include <stdio.h>

#include "interface/vcsm/user-vcsm.h"

#include "gles3.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "static_array.h"
#include "gpu_texture.h"
#include "gpu_framebuffer.h"

#define GPU_TEXTURE_MAX_COUNT 200

typedef struct {
  uint8_t refcount;  /* reference count */
  int width, height;

  signed slot:4;      /* number for multitexture binding */
  unsigned format:4;
  unsigned is_texture:1;

  GLuint bindcode;    /* opengl identifier for texture */
  GLenum target;      /* GL_TEXTURE_2D/GL_TEXTURE_CUBE_MAP_xx/ use it for bind/unbind */

  //int fb_attachment[GPU_TEX_MAX_FBO_ATTACHED];
  //GPUFrameBuffer *fb[GPU_TEX_MAX_FBO_ATTACHED];
} GPUTexture;

static GPUTexture textures[GPU_TEXTURE_MAX_COUNT];
static ID_t next_deleted_texture;

static struct egl_image_brcm_vcsm_info vcsm_info;
static EGLImageKHR eglFbImage = EGL_NO_IMAGE_KHR;
static unsigned char *vcsm_buffer;

static const GLenum data_format[] = {
  0,
  GL_LUMINANCE,
  GL_LUMINANCE_ALPHA,
  GL_RGB,
  GL_RGBA,
  GL_STENCIL_INDEX8,
  GL_DEPTH_COMPONENT16,
  GL_DEPTH_COMPONENT24_OES,
  GL_DEPTH_COMPONENT32_OES
};

static inline GLenum gpu_get_gl_dataformat(GPUTextureFormat data_type)
{
  return data_format[data_type];
}

/* ------ Memory Management ------- */
/* Records every texture allocation / free
 * to estimate the Texture Pool Memory consumption */
static GLuint memory_usage;

static GLuint gpu_texture_memory_footprint_compute(GPUTexture *tex)
{

  const GLuint bytes = (tex->format < GPU_STENCIL8) ? tex->format : tex->format - GPU_RGBA8;
  GLuint size = bytes * tex->width * tex->height;
  if (tex->target == GL_TEXTURE_CUBE_MAP) size *= 6;
  printf("texture size: %u\n", size);
  
  return size;
}

static void gpu_texture_memory_footprint_add(GPUTexture *tex)
{
  memory_usage += gpu_texture_memory_footprint_compute(tex);
  printf("Texture Memory: %i\n", memory_usage);
}

static void gpu_texture_memory_footprint_remove(GPUTexture *tex)
{
  memory_usage -= gpu_texture_memory_footprint_compute(tex);
}

static inline ID_t find_deleted_texture_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_texture, textures,
                            GPU_TEXTURE_MAX_COUNT, GPUTexture, "Texture");;
}

static GPUTexture *get_texture(ID_t id)
{
  if ((id < 0) | (id >= GPU_TEXTURE_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Texture id, using default id: 0\n");
  }
    
  return textures + id;
}

static void init_renderbuffer(GPUTexture *tex)
{
  tex->is_texture = 0;
  tex->target = GL_RENDERBUFFER;
  if(!tex->bindcode) glGenRenderbuffers(1, &tex->bindcode);
  glBindRenderbuffer(GL_RENDERBUFFER, tex->bindcode);
  const GLenum data_format = gpu_get_gl_dataformat(tex->format);
  glRenderbufferStorage(GL_RENDERBUFFER, data_format, tex->width, tex->height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

static void init_texture(GPUTexture *tex, const void *pixels)
{
  tex->is_texture = 1;
  tex->target = GL_TEXTURE_2D;
  /* Generate Texture object */
  if(!tex->bindcode) glGenTextures(1, &tex->bindcode);

  glBindTexture(GL_TEXTURE_2D, tex->bindcode);

  const GLenum data_format = gpu_get_gl_dataformat(tex->format);
  /* Upload Texture */
  glTexImage2D(GL_TEXTURE_2D, 0, data_format, tex->width, tex->height, 0, data_format, GL_UNSIGNED_BYTE, pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, 0);

}

static void init_texture_vcsm(GPUTexture *tex)
{
  tex->is_texture = 1;
  tex->target = GL_TEXTURE_2D;
  /* Generate Texture object */
  if(!tex->bindcode) glGenTextures(1, &tex->bindcode);

  glBindTexture(GL_TEXTURE_2D, tex->bindcode);
  if (eglFbImage == EGL_NO_IMAGE_KHR) {
    vcsm_init();
    vcsm_info.width = tex->width;
    vcsm_info.height = tex->height;
    eglFbImage = eglCreateImageKHR(eglGetDisplay(EGL_DEFAULT_DISPLAY), EGL_NO_CONTEXT,
            EGL_IMAGE_BRCM_VCSM, &vcsm_info, NULL);
  }

  if (eglFbImage == EGL_NO_IMAGE_KHR || vcsm_info.vcsm_handle == 0) {
    printf("*** Failed to create EGL VCSM image\n");
  }
  else {
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, eglFbImage);
  }
}

ID_t GPU_texture_create(const int w, const int h,
            const GPUTextureFormat tex_format, const void *pixels)
{
  const ID_t id = find_deleted_texture_id();
  GPUTexture *const tex = get_texture(id);

  tex->width = w;
  tex->height = h;
  tex->slot = -1;
  tex->refcount = 1;
  tex->format = tex_format;

  if (tex_format < GPU_STENCIL8)
    init_texture(tex, pixels);
  else {
    if (tex_format < GPU_VCSM)
      init_renderbuffer(tex);
    else
      init_texture_vcsm(tex);
  }

  gpu_texture_memory_footprint_add(tex);

  printf("New glObjID: %i\n", tex->bindcode);

  return id;
}

void GPU_texture_sub_image(const ID_t id, const GLint xoffset,
  const GLint yoffset, const GLsizei width,const GLsizei height, const void *pixels)
{
  GPUTexture * const tex = get_texture(id);
  if (tex->format < GPU_STENCIL8) {
    const GLenum data_format = gpu_get_gl_dataformat(tex->format);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, tex->bindcode);
    glTexSubImage2D(tex->target, 0, xoffset, yoffset, width, height,
                    data_format, GL_UNSIGNED_BYTE, pixels);
  }
}

void GPU_texture_bind(const ID_t id, const int slot)
{
  if (slot >= 8) {
    printf("Not enough texture slots.\n");
    return;
  }

  glActiveTexture(GL_TEXTURE0 + slot);

  GPUTexture * const tex = get_texture(id);
  if (tex->bindcode != 0)
    glBindTexture(tex->target, tex->bindcode);
  tex->slot = slot;
}

void GPU_texture_mipmap(const ID_t id)
{
  GPUTexture * const tex = get_texture(id);
  if (tex->format < GPU_STENCIL8) {
    glBindTexture(GL_TEXTURE_2D, tex->bindcode);
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    glGenerateMipmap(GL_TEXTURE_2D);  
  }
}

void GPU_texture_unbind(const ID_t id)
{
  GPUTexture * const tex = get_texture(id);
  if (tex->slot == -1)
    return;

  glActiveTexture(GL_TEXTURE0 + tex->slot);
  glBindTexture(tex->target, 0);

  tex->slot = -1;
}

void GPU_texture_free(const ID_t id)
{
  GPUTexture * const tex = get_texture(id);
  tex->refcount--;

  if (tex->refcount < 0)
    printf("GPUTexture: negative refcount\n");

  if (tex->refcount == 0) {
    GPU_framebuffer_texture_detach_all(id);

    //glDeleteTextures(1, &tex->bindcode);//GPU_tex_free(tex->bindcode);

    gpu_texture_memory_footprint_remove(tex);

  }
}

int GPU_texture_bound_slot(const ID_t id)
{
  return get_texture(id)->slot;
}


void GPU_texture_ref(const ID_t id)
{
  get_texture(id)->refcount++;
}

int GPU_texture_target(const ID_t id)
{
  return get_texture(id)->target;
}

int GPU_texture_width(const ID_t id)
{
  return get_texture(id)->width;
}

int GPU_texture_height(const ID_t id)
{
  return get_texture(id)->height;
}

GPUTextureFormat GPU_texture_format(const ID_t id)
{
  return get_texture(id)->format;
}

GLboolean GPU_texture_cube(const ID_t id)
{
  return (get_texture(id)->target == GL_TEXTURE_CUBE_MAP);
}

GLuint GPU_texture_opengl_bindcode(const ID_t id)
{
  return get_texture(id)->bindcode;
}

unsigned char *GPU_texture_vcsm_lock(void)
{
  VCSM_CACHE_TYPE_T cache_type;

  if (!vcsm_buffer && vcsm_info.vcsm_handle) {
    vcsm_buffer = (unsigned char *) vcsm_lock_cache(vcsm_info.vcsm_handle, VCSM_CACHE_TYPE_HOST, &cache_type);
  }

  return vcsm_buffer;
}

void GPU_texture_vcsm_unlock(void)
{
  if (vcsm_buffer) {
    vcsm_unlock_ptr(vcsm_buffer);
    vcsm_buffer = 0;
  }
}
