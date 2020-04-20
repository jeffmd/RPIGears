// gpu_framebuffer.c

#include <stdio.h>
#include <stdint.h>

#include "gles3.h"
#include "gpu_texture.h"
#include "static_array.h"

#define GPU_FRAMEBUFFER_MAX_COUNT 200

typedef enum {
  GPU_FB_DEPTH_ATTACHMENT = 0,
  GPU_FB_STENCIL_ATTACHMENT,
  GPU_FB_COLOR_ATTACHMENT0,

  // Number of maximum output slots.
  GPU_FB_MAX_ATTACHEMENT
} GPUAttachmentType;

typedef struct {
  uint8_t refcount;
  //GPUContext *ctx;
  uint8_t object;
  uint8_t dirty_flag;
  uint16_t width, height;
  short attachments[GPU_FB_MAX_ATTACHEMENT];
} GPUFrameBuffer;

#define GPU_FB_ATTACHEMENT_IS_DIRTY(flag, type) ((flag & (1 << type)) != 0)
#define GPU_FB_ATTACHEMENT_SET_DIRTY(flag, type) (flag |= (1 << type))

static GPUFrameBuffer framebuffers[GPU_FRAMEBUFFER_MAX_COUNT];
static short next_deleted_framebuffer;
static short active_framebuffer;

static inline short find_deleted_framebuffer_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_framebuffer, framebuffers, 
                            GPU_FRAMEBUFFER_MAX_COUNT, GPUFrameBuffer, "Frame Buffer");
}

static GPUFrameBuffer *get_framebuffer(short id)
{
  if ((id < 0) | (id >= GPU_FRAMEBUFFER_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Frame buffer id, using default id: 0\n");
  }
    
  return framebuffers + id;
}

static GLenum convert_attachment_type_to_gl(const GPUAttachmentType type)
{
  static const GLenum table[] = {
    [GPU_FB_COLOR_ATTACHMENT0] = GL_COLOR_ATTACHMENT0,
    [GPU_FB_STENCIL_ATTACHMENT] = GL_STENCIL_ATTACHMENT,
    [GPU_FB_DEPTH_ATTACHMENT] = GL_DEPTH_ATTACHMENT
  };
  return table[type];
}

static GPUAttachmentType attachment_type_from_tex(const int tex)
{
  switch (GPU_texture_format(tex)) {
    case GPU_DEPTH32:
    case GPU_DEPTH24:
    case GPU_DEPTH16:
      return GPU_FB_DEPTH_ATTACHMENT;

    case GPU_STENCIL8:
      return GPU_FB_STENCIL_ATTACHMENT;
    default:
      return GPU_FB_COLOR_ATTACHMENT0;
  }
}

short GPU_framebuffer_active(void)
{
  return active_framebuffer;
}

static void gpu_framebuffer_set_active(const short id)
{
  active_framebuffer = id;
}

short GPU_framebuffer_create(void)
{
  /* We generate the FB object later at first use in order to
   * create the framebuffer in the right opengl context. */
  const int id = find_deleted_framebuffer_id();
  get_framebuffer(id)->refcount = 1;

  return id;
}

static void gpu_framebuffer_init(GPUFrameBuffer *fb)
{
  GLuint obj;
  glGenFramebuffers(1, &obj);
  fb->object = obj;
}

void GPU_framebuffer_free(const short id)
{
  GPUFrameBuffer *const fb = get_framebuffer(id);
  
  fb->refcount--;

  if (fb->refcount < 0)
    printf("GPUFramebuffer: negative refcount\n");

  if (fb->refcount == 0) {
    for (GPUAttachmentType type = 0; type < GPU_FB_MAX_ATTACHEMENT; type++) {
      fb->attachments[type] = 0;
    }

    if (GPU_framebuffer_active() == id) {
      gpu_framebuffer_set_active(0);
    }
    /* This restores the framebuffer if it was bound */
    //GLuint obj = fb->object;
    //glDeleteFramebuffers(1, &obj);

  }
}

void GPU_framebuffer_texture_detach(const short id, const short tex)
{
  GPUFrameBuffer *const fb = get_framebuffer(id);

  const GPUAttachmentType at_type = attachment_type_from_tex(tex);

  if (fb->attachments[at_type] == tex) {
    fb->attachments[at_type] = 0;
    GPU_FB_ATTACHEMENT_SET_DIRTY(fb->dirty_flag, at_type);
  }
}

void GPU_framebuffer_texture_detach_all(const short tex)
{
  for (int id = 1; id < GPU_FRAMEBUFFER_MAX_COUNT; id++) {
    GPU_framebuffer_texture_detach(id, tex);
  }

}

void GPU_framebuffer_texture_attach(const short id, const short tex)
{
  GPUFrameBuffer *const fb = get_framebuffer(id);
  const GPUAttachmentType type = attachment_type_from_tex(tex);

  if (fb->attachments[type] != tex)
  {
    fb->attachments[type] = tex;
    GPU_FB_ATTACHEMENT_SET_DIRTY(fb->dirty_flag, type);
  }

}

static void gpu_framebuffer_attachment_attach(const short tex, const GPUAttachmentType attach_type)
{
  const int tex_bind = GPU_texture_opengl_bindcode(tex);
  const GLenum target = GPU_texture_target(tex);
  const GLenum gl_attachment = convert_attachment_type_to_gl(attach_type);


  if (gl_attachment == GL_COLOR_ATTACHMENT0) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, gl_attachment, target, tex_bind, 0);
  }
  else {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, gl_attachment, target, tex_bind);
  }
}

static void gpu_framebuffer_attachment_detach(const GPUAttachmentType attach_type)
{
  const GLenum gl_attachment = convert_attachment_type_to_gl(attach_type);

  if (gl_attachment == GL_COLOR_ATTACHMENT0) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, gl_attachment, 0, 0, 0);
  }
  else {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, gl_attachment, 0, 0);
  }
}

static void gpu_framebuffer_update_attachments(GPUFrameBuffer *fb)
{

  //BLI_assert(GPU_framebuffer_active_get() == fb);

  /* Update attachments */
  for (GPUAttachmentType type = 0; type < GPU_FB_MAX_ATTACHEMENT; ++type) {


    if (!GPU_FB_ATTACHEMENT_IS_DIRTY(fb->dirty_flag, type)) {
      continue;
    }
    else if (fb->attachments[type] != 0) {
      gpu_framebuffer_attachment_attach(fb->attachments[type], type);

      fb->width = GPU_texture_width(fb->attachments[type]);
      fb->height = GPU_texture_height(fb->attachments[type]);
    }
    else {
      gpu_framebuffer_attachment_detach(type);
    }
  }
  fb->dirty_flag = 0;

}

void GPU_framebuffer_done(void)
{
  const GLenum attachments[3] = { GL_COLOR_EXT, GL_DEPTH_EXT, GL_STENCIL_EXT };
  glDiscardFramebufferEXT( GL_FRAMEBUFFER , 3, attachments);
}

void GPU_framebuffer_bind(const short id)
{
  GPU_framebuffer_done();
  GPUFrameBuffer *const fb = get_framebuffer(id);

  if (fb->object == 0)
    gpu_framebuffer_init(fb);

  //if (GPU_framebuffer_active_get() != fbID)
    glBindFramebuffer(GL_FRAMEBUFFER, fb->object);

  gpu_framebuffer_set_active(id);

  if (fb->dirty_flag)
    gpu_framebuffer_update_attachments(fb);

  glViewport(0, 0, fb->width, fb->height);
}

