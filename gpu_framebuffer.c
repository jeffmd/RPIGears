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
  uint16_t attachments[GPU_FB_MAX_ATTACHEMENT];
} GPUFrameBuffer;

#define GPU_FB_ATTACHEMENT_IS_DIRTY(flag, type) ((flag & (1 << type)) != 0)
#define GPU_FB_ATTACHEMENT_SET_DIRTY(flag, type) (flag |= (1 << type))

static GPUFrameBuffer framebuffers[GPU_FRAMEBUFFER_MAX_COUNT];
static uint16_t next_deleted_framebuffer = 0;
static uint16_t active_framebuffer;

static GLuint find_deleted_framebuffer(void)
{
  if(next_deleted_framebuffer == 0) next_deleted_framebuffer = 1;

  for (GLuint id = next_deleted_framebuffer; id < GPU_FRAMEBUFFER_MAX_COUNT; id++) {
    if (framebuffers[id].refcount == 0) {
      next_deleted_framebuffer = id + 1;
      return id;
    }
}

  printf("WARNING: No Frame Buffers available\n");
  return GPU_FRAMEBUFFER_MAX_COUNT - 1;
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

static GPUAttachmentType attachment_type_from_tex(GLuint texID)
{
  switch (GPU_texture_format(texID)) {
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

GLuint GPU_framebuffer_active_get(void)
{
  return active_framebuffer;
}

static void gpu_framebuffer_current_set(GLuint fbID)
{
  active_framebuffer = fbID;
}

GLuint GPU_framebuffer_create(void)
{
  /* We generate the FB object later at first use in order to
   * create the framebuffer in the right opengl context. */
  const GLuint framebufferID = find_deleted_framebuffer();
  GPUFrameBuffer *fb = &framebuffers[framebufferID];
  fb->refcount = 1;

  return framebufferID;
}

static void gpu_framebuffer_init(GPUFrameBuffer *fb)
{
  GLuint obj;
  glGenFramebuffers(1, &obj);
  fb->object = obj;
}

void GPU_framebuffer_free(GLuint framebufferID)
{
  GPUFrameBuffer *fb = &framebuffers[framebufferID];

  fb->refcount--;

  if (fb->refcount < 0)
    printf("GPUFramebuffer: negative refcount\n");

  if (fb->refcount == 0) {
    for (GPUAttachmentType type = 0; type < GPU_FB_MAX_ATTACHEMENT; type++) {
      fb->attachments[type] = 0;
    }

    if (GPU_framebuffer_active_get() == framebufferID) {
      gpu_framebuffer_current_set(0);
    }
    /* This restores the framebuffer if it was bound */
    //GLuint obj = fb->object;
    //glDeleteFramebuffers(1, &obj);

  }
}

void GPU_framebuffer_texture_detach(GLuint fbID, GLuint texID)
{
  const GPUAttachmentType at_type = attachment_type_from_tex(texID);
  GPUFrameBuffer *fb = &framebuffers[fbID];

  if (fb->attachments[at_type] == texID) {
    fb->attachments[at_type] = 0;
    GPU_FB_ATTACHEMENT_SET_DIRTY(fb->dirty_flag, at_type);
  }
}

void GPU_framebuffer_texture_detach_all(GLuint texID)
{

  for (GLuint id = 0; id < GPU_FRAMEBUFFER_MAX_COUNT; id++) {
    GPU_framebuffer_texture_detach(id, texID);
  }

}

void GPU_framebuffer_texture_attach(GLuint fbID, GLuint texID)
{

  const GPUAttachmentType type = attachment_type_from_tex(texID);
  GPUFrameBuffer *fb = &framebuffers[fbID];

  if (fb->attachments[type] != texID)
  {
    fb->attachments[type] = texID;
    GPU_FB_ATTACHEMENT_SET_DIRTY(fb->dirty_flag, type);
  }

}

static void gpu_framebuffer_attachment_attach(GLuint texID, GPUAttachmentType attach_type)
{
  const int tex_bind = GPU_texture_opengl_bindcode(texID);
  const GLenum target = GPU_texture_target(texID);
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

void GPU_framebuffer_bind(GLuint fbID)
{
  GPUFrameBuffer *fb = &framebuffers[fbID];

  if (fb->object == 0)
    gpu_framebuffer_init(fb);

  //if (GPU_framebuffer_active_get() != fbID)
    glBindFramebuffer(GL_FRAMEBUFFER, fb->object);

  gpu_framebuffer_current_set(fbID);

  if (fb->dirty_flag)
    gpu_framebuffer_update_attachments(fb);

  glViewport(0, 0, fb->width, fb->height);
}

