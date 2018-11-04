// gpu_framebuffer.c

#include <stdio.h>

#include "gles3.h"

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
	//uint16_t dirty_flag;
	int width, height;
	unsigned type:2;
	uint16_t color_buffer;
    uint16_t depth_renderBuffer;
    uint16_t stencil_renderBuffer; 
} GPUFrameBuffer;
