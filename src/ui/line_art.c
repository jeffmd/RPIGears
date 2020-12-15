// line_art.c

#include <stdio.h>
#include <stdint.h>

#include "gles3.h"
#include "gpu_vertex_format.h"
#include "gpu_vertex_buffer.h"
#include "gpu_uniform_buffer.h"
#include "gpu_batch.h"
#include "shaders.h"

enum {
  ATTR_POSITION,
};

static short line_art_batch; 
static short vformat;

static GLfloat proj_matrix[4] = {1.0f/1280.0f, 1.0f/1024.0f, -0.055f, -0.01f};

static short get_vformat(void)
{
  if (!vformat) {
    vformat = GPU_vertex_format_create();
    GPU_vertex_format_add_attribute(vformat, "position", 2, GL_FLOAT);
  }

  return vformat;
}

static short get_line_art_batch(void)
{
  if (!line_art_batch) {
    line_art_batch = GPU_batch_create();
    const short ubuff = GPU_batch_uniform_buffer(line_art_batch);
    GPU_uniformbuffer_add_4f(ubuff, "ProjMat", proj_matrix);    

    const short vbuff = GPU_batch_vertex_buffer(line_art_batch);
    GPU_vertbuf_set_vertex_format(vbuff, get_vformat());
    GPU_vertbuf_set_add_count(vbuff, 100);
  }

  return line_art_batch;
}


short Line_Art_create(void)
{
  short line_art = GPU_batch_part_create(get_line_art_batch());
  
  return line_art;
}

static short get_vbuff()
{
  return GPU_batch_vertex_buffer(get_line_art_batch());
}

void Line_Art_end(const short id)
{
  GPU_batch_part_end(id);
}

void Line_Art_add(const float x, const float y)
{
  GPU_vertbuf_add_2(get_vbuff(), ATTR_POSITION, x, y);
}

void Line_Art_draw(const short batch_part)
{
  Shaders_bind_line_art();
  GPU_uniformbuffer_activate(0);
  GPU_batch_part_draw(batch_part, GL_LINE_STRIP, 1);
}

void Line_Art_set_scale_offset(float scale_x, float scale_y, float offset_x, float offset_y)
{
  const float cx = 1.0f / 1280.0f;
  const float cy = 1.0f / 1024.0f;

  const float sx = scale_x * cx;
  const float sy = scale_y * cy;

  const float a1 = offset_x * cx;
  const float a2 = offset_y * cy;

  proj_matrix[0] = sx;
  proj_matrix[1] = sy;
  proj_matrix[2] = a1;
  proj_matrix[3] = a2;
}
