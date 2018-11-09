// gpu_framebuffer.c

#include <stdio.h>
#include <stdint.h>

#include "gles3.h"

#define GPU_FRAMEBUFFER_MAX_COUNT 5

typedef enum {
	GPU_FB_DEPTH_ATTACHMENT = 0,
	GPU_FB_STENCIL_ATTACHMENT,
	GPU_FB_COLOR_ATTACHMENT0,

	// Number of maximum output slots.
	GPU_FB_MAX_ATTACHEMENT
} GPUAttachmentType;

typedef struct {
	GLuint objID;
} GPUAttachment;

typedef struct {
	//GPUContext *ctx;
	uint16_t object;
	uint8_t refcount;
	//uint16_t dirty_flag;
	int width, height;
	unsigned type:2;
	uint16_t attachments[GPU_FB_MAX_ATTACHEMENT];
} GPUFrameBuffer;

static GPUFrameBuffer framebuffers[GPU_FRAMEBUFFER_MAX_COUNT];
static uint16_t next_deleted_framebuffer = 0;

static GLuint find_deleted_framebuffer(void)
{
  
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
