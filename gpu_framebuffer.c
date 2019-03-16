// gpu_framebuffer.c

#include <stdio.h>
#include <stdint.h>

#include "gles3.h"
#include "gpu_texture.h"

#define GPU_FRAMEBUFFER_MAX_COUNT 200

typedef enum {
  GPU_FB_DEPTH_ATTACHMENT = 0,
  GPU_FB_STENCIL_ATTACHMENT,
  GPU_FB_COLOR_ATTACHMENT0,

  // Number of maximum output slots.
  GPU_FB_MAX_ATTACHEMENT
} GPUAttachmentType;

typedef struct {
  //GPUContext *ctx;
  uint8_t object;
  uint8_t refcount;
  uint8_t dirty_flag;
  uint16_t width, height;
  GPUTexture *attachments[GPU_FB_MAX_ATTACHEMENT];
} GPUFrameBuffer;

#define GPU_FB_ATTACHEMENT_IS_DIRTY(flag, type) ((flag & (1 << type)) != 0)
#define GPU_FB_ATTACHEMENT_SET_DIRTY(flag, type) (flag |= (1 << type))

static GPUFrameBuffer framebuffers[GPU_FRAMEBUFFER_MAX_COUNT];
static GPUFrameBuffer *next_deleted_framebuffer = 0;
static GPUFrameBuffer *active_framebuffer;

static GPUFrameBuffer *find_deleted_framebuffer(void)
{
  GPUFrameBuffer *fb = next_deleted_framebuffer;
  const GPUFrameBuffer *max_fb = framebuffers + GPU_FRAMEBUFFER_MAX_COUNT;
  
  if((fb <= framebuffers) | (fb >= max_fb))
    fb = framebuffers + 1;

  for ( ; fb < max_fb; fb++) {
    if (fb->refcount == 0) {
      next_deleted_framebuffer = fb + 1;
      break;
    }
  }

  if (fb == max_fb) {
    printf("WARNING: No Frame Buffers available\n");
    fb = framebuffers;
  }
    
  return fb;
}

static GLenum convert_attachment_type_to_gl(GPUAttachmentType type)
{
  static const GLenum table[] = {
    [GPU_FB_COLOR_ATTACHMENT0] = GL_COLOR_ATTACHMENT0,
    [GPU_FB_STENCIL_ATTACHMENT] = GL_STENCIL_ATTACHMENT,
    [GPU_FB_DEPTH_ATTACHMENT] = GL_DEPTH_ATTACHMENT
  };
  return table[type];
}

static GPUAttachmentType attachment_type_from_tex(GPUTexture *tex)
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

GPUFrameBuffer *GPU_framebuffer_active_get(void)
{
  return active_framebuffer;
}

static void gpu_framebuffer_current_set(GPUFrameBuffer *fb)
{
  active_framebuffer = fb;
}

GPUFrameBuffer *GPU_framebuffer_create(void)
{
  /* We generate the FB object later at first use in order to
   * create the framebuffer in the right opengl context. */
  GPUFrameBuffer *fb = find_deleted_framebuffer();
  fb->refcount = 1;

  return fb;
}

static void gpu_framebuffer_init(GPUFrameBuffer *fb)
{
  GLuint obj;
  glGenFramebuffers(1, &obj);
  fb->object = obj;
}

void GPU_framebuffer_free(GPUFrameBuffer *fb)
{

  fb->refcount--;

  if (fb->refcount < 0)
    printf("GPUFramebuffer: negative refcount\n");

  if (fb->refcount == 0) {
    for (GPUAttachmentType type = 0; type < GPU_FB_MAX_ATTACHEMENT; type++) {
      fb->attachments[type] = 0;
    }

    if (GPU_framebuffer_active_get() == fb) {
      gpu_framebuffer_current_set(0);
    }
    /* This restores the framebuffer if it was bound */
    //GLuint obj = fb->object;
    //glDeleteFramebuffers(1, &obj);

  }
}

void GPU_framebuffer_texture_detach(GPUFrameBuffer *fb, GPUTexture *tex)
{
  const GPUAttachmentType at_type = attachment_type_from_tex(tex);

  if (fb->attachments[at_type] == tex) {
    fb->attachments[at_type] = 0;
    GPU_FB_ATTACHEMENT_SET_DIRTY(fb->dirty_flag, at_type);
  }
}

void GPU_framebuffer_texture_detach_all(GPUTexture *tex)
{
  const GPUFrameBuffer *max_fb = framebuffers + GPU_FRAMEBUFFER_MAX_COUNT;
  for (GPUFrameBuffer *fb = framebuffers; fb < max_fb; fb++) {
    GPU_framebuffer_texture_detach(fb, tex);
  }

}

void GPU_framebuffer_texture_attach(GPUFrameBuffer *fb, GPUTexture *tex)
{

  const GPUAttachmentType type = attachment_type_from_tex(tex);

  if (fb->attachments[type] != tex)
  {
    fb->attachments[type] = tex;
    GPU_FB_ATTACHEMENT_SET_DIRTY(fb->dirty_flag, type);
  }

}

static void gpu_framebuffer_attachment_attach(GPUTexture *tex, GPUAttachmentType attach_type)
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

static void gpu_framebuffer_attachment_detach(GPUAttachmentType attach_type)
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

void GPU_framebuffer_bind(GPUFrameBuffer *fb)
{
  if (fb->object == 0)
    gpu_framebuffer_init(fb);

  //if (GPU_framebuffer_active_get() != fbID)
    glBindFramebuffer(GL_FRAMEBUFFER, fb->object);

  gpu_framebuffer_current_set(fb);

  if (fb->dirty_flag)
    gpu_framebuffer_update_attachments(fb);

  glViewport(0, 0, fb->width, fb->height);
}

