// gpu_texture.c

#include <stdio.h>

#include "gles3.h"
#include "gpu_framebuffer.h"
#include "gpu_texture.h"


#define GPU_TEXTURE_MAX_COUNT 200

typedef struct {
  int width, height;

  unsigned refcount:5;  /* reference count */
  signed slot:4;      /* number for multitexture binding */
  unsigned format:4;
  unsigned is_texture:1;

  GLuint bindcode;    /* opengl identifier for texture */
  GLenum target;      /* GL_TEXTURE_2D/GL_TEXTURE_CUBE_MAP_xx/ use it for bind/unbind */

  //int fb_attachment[GPU_TEX_MAX_FBO_ATTACHED];
  //GPUFrameBuffer *fb[GPU_TEX_MAX_FBO_ATTACHED];
} GPUTexture;

static GPUTexture textures[GPU_TEXTURE_MAX_COUNT];
static uint16_t next_deleted_texture = 0;

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

static GLuint gpu_texture_memory_footprint_compute(const GLuint texID)
{

  GPUTexture *tex = &textures[texID];
  const GLuint bytes = (tex->format < GPU_STENCIL8) ? tex->format : tex->format - GPU_RGBA8;
  GLuint size = bytes * tex->width * tex->height;
  if (tex->target == GL_TEXTURE_CUBE_MAP) size *= 6;

  return size;
}

static void gpu_texture_memory_footprint_add(const GLuint texID)
{
  memory_usage += gpu_texture_memory_footprint_compute(texID);
  printf("Texture Memory: %i\n", memory_usage);
}

static void gpu_texture_memory_footprint_remove(const GLuint texID)
{
  memory_usage -= gpu_texture_memory_footprint_compute(texID);
}

static GLuint find_deleted_texture(void)
{
  GLuint id = next_deleted_texture;
  
  if((id == 0) | (id >= GPU_TEXTURE_MAX_COUNT))
    id = 1;

  for ( ; id < GPU_TEXTURE_MAX_COUNT; id++) {
    if (textures[id].refcount == 0) {
      next_deleted_texture = id + 1;
      break;
    }
  }

  if (id == GPU_TEXTURE_MAX_COUNT) {
    printf("WARNING: No Textures available\n");
    id = 0;
  }

  return id;
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
  if(!tex->bindcode) glGenTextures(1, &tex->bindcode);//GPU_tex_alloc();

  //if (!tex->bindcode) {
    //printf("GPUTexture: texture create failed\n");
    //GPU_texture_free(tex);
    //return NULL;
  //}

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

GLuint GPU_texture_create_2D(const int w, const int h,
            const GPUTextureFormat tex_format, const void *pixels)
{
  const GLuint texID = find_deleted_texture();

  GPUTexture *tex = &textures[texID];

  tex->width = w;
  tex->height = h;
  tex->slot = -1;
  tex->refcount = 1;
  tex->format = tex_format;

  if (tex_format < GPU_STENCIL8)
    init_texture(tex, pixels);
  else
    init_renderbuffer(tex);

  gpu_texture_memory_footprint_add(texID);

  printf("New Texture ID:%i glObjID: %i\n", texID, tex->bindcode);

  return texID;
}

void GPU_texture_bind(const GLuint texID, const int slot)
{

  if (slot >= 8) {
    printf("Not enough texture slots.\n");
    return;
  }

  glActiveTexture(GL_TEXTURE0 + slot);

  GPUTexture *tex = &textures[texID];

  if (tex->bindcode != 0)
    glBindTexture(tex->target, tex->bindcode);
  //else
  //  GPU_invalid_tex_bind(tex->target_base);
  //printf("bind texture: %i\n", tex->bindcode);
  tex->slot = slot;
}

void GPU_texture_unbind(const GLuint texID)
{
  GPUTexture *tex = &textures[texID];

  if (tex->slot == -1)
    return;

  glActiveTexture(GL_TEXTURE0 + tex->slot);
  glBindTexture(tex->target, 0);

  tex->slot = -1;
}

void GPU_texture_free(const GLuint texID)
{
  GPUTexture *tex = &textures[texID];

  tex->refcount--;

  if (tex->refcount < 0)
    printf("GPUTexture: negative refcount\n");

  if (tex->refcount == 0) {
    GPU_framebuffer_texture_detach_all(texID);

    //glDeleteTextures(1, &tex->bindcode);//GPU_tex_free(tex->bindcode);

    gpu_texture_memory_footprint_remove(texID);

  }
}

int GPU_texture_bound_slot(const GLuint texID)
{
  return textures[texID].slot;
}


void GPU_texture_ref(const GLuint texID)
{
  textures[texID].refcount++;
}

int GPU_texture_target(const GLuint texID)
{
  return textures[texID].target;
}

int GPU_texture_width(const GLuint texID)
{
  return textures[texID].width;
}

int GPU_texture_height(const GLuint texID)
{
  return textures[texID].height;
}

GPUTextureFormat GPU_texture_format(const GLuint texID)
{
  return textures[texID].format;
}

GLboolean GPU_texture_cube(const GLuint texID)
{
  return (textures[texID].target == GL_TEXTURE_CUBE_MAP);
}

GLuint GPU_texture_opengl_bindcode(const GLuint texID)
{
  return textures[texID].bindcode;
}
