// gpu_vertex_format.c

#include <stdio.h>

#include "gles3.h"
#include "fp16.h"
#include "gpu_shader.h"
#include "static_array.h"

typedef struct {
  GLenum type;             // GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_FIXED, or GL_FLOAT
  uint8_t size;            // Components per element (1,2,3,4)
  uint8_t normalized;      // Fixed-point values are normalized when converted to floats
  const char *name;        // attribute name in vertex shader
  GLuint offset;           // offset from start of data
} VertAttribute;

typedef struct {
  uint8_t active;           // not zero if vertex format is not deleted
  VertAttribute vertex_attributes[VERT_ATTRIB_MAX];
  uint8_t attribute_count;  // count of active attributes in vertex_attributes array
  uint8_t stride;           // size in bytes of vertex data - only valid after attributes added and ready is not zero
} GPUVertFormat;

#define VERT_FORMAT_MAX_COUNT 5

static GPUVertFormat vert_formats[VERT_FORMAT_MAX_COUNT];
static GPUVertFormat *next_deleted_vert_format = 0;

static inline GPUVertFormat *find_deleted_vert_format(void)
{
  return ARRAY_FIND_DELETED(next_deleted_vert_format, vert_formats,
                            VERT_FORMAT_MAX_COUNT, "vertex format");
}

void GPU_vertex_format_init(GPUVertFormat *vformat)
{
  vformat->attribute_count = 0;
  vformat->stride = 0;
}

GPUVertFormat *GPU_vertex_format_create(void)
{
  GPUVertFormat *vformat = find_deleted_vert_format();

  vformat->active = 1;
  GPU_vertex_format_init(vformat);
  printf("New vertex format ID:%p\n", vformat);

	return vformat;
}


// add attribute to vertex format
void GPU_vertex_format_add_attribute(GPUVertFormat *vformat, const char *name, const GLint size, const GLenum type)
{
  VertAttribute *vattr = &vformat->vertex_attributes[vformat->attribute_count];
  
  vattr->type = type;
  vattr->size = size;
  vattr->normalized = GL_FALSE;
  vattr->name = name;
                               
  vformat->attribute_count++;
  vformat->stride = 0;
}

void GPU_vertex_format_delete(GPUVertFormat *vformat)
{
  vformat->active = 0;
  
  if (vformat < next_deleted_vert_format)
    next_deleted_vert_format = vformat; 
}

static GLuint get_type_byte_size(const GLenum type)
{
  switch(type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
      return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
      return 2;
    default:
      return 4;
  }
}

GLuint GPU_vertex_format_stride(GPUVertFormat *vformat)
{
  if (!vformat->stride) {
    const GLuint max_Idx = vformat->attribute_count;
    // calculate stride
    for (GLuint Idx = 0; Idx < max_Idx; Idx++) {
      vformat->vertex_attributes[Idx].offset = vformat->stride;
      vformat->stride += get_type_byte_size(vformat->vertex_attributes[Idx].type) * vformat->vertex_attributes[Idx].size;
    }
  }
  
  return vformat->stride;
}

GLuint GPU_vertex_format_offset(GPUVertFormat *vformat, const GLuint idx)
{
  return vformat->vertex_attributes[idx].offset;
}

GLuint GPU_vertex_format_attribute_count(GPUVertFormat *vformat)
{
  return vformat->attribute_count;
}

void GPU_vertex_format_add_4(GPUVertFormat *vformat, const GLuint attribute_id, GLvoid *attr_data, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4)
{
  if (attr_data) {
    VertAttribute *vattr = &vformat->vertex_attributes[attribute_id];
    const GLuint size = vattr->size;
    
    if (vattr->type == GL_FLOAT) {
      GLfloat *data = attr_data;
      data[0] = val1;
      
      if (size > 1)
        data[1] = val2;
      if (size > 2)
        data[2] = val3;
      if (size > 3)
        data[3] = val4;
    }
    else if (vattr->type == GL_HALF_FLOAT_OES) {
      GLshort *data = attr_data;
      data[0] = f16(val1);
      if (size > 1)
        data[1] = f16(val2);
      if (size > 2)
        data[2] = f16(val3);
      if (size > 3)
        data[3] = f16(val4);
    }
  }
  else {
    printf("ERROR: vertex format cannot add values to buffer, buffer pointer is NULL\n");
  }
}


void GPU_vertex_format_bind(GPUVertFormat *vformat, GLvoid *data)
{
  const GLuint max_Idx = vformat->attribute_count;
  // enable active genaric attributes 
  for (GLuint Idx = 0; Idx < max_Idx; Idx++) {
    VertAttribute *vattr = &vformat->vertex_attributes[Idx];

    GLint loc = GPU_get_active_attribute_location(vattr->name);
    if (loc >= 0) {
      glEnableVertexAttribArray(loc);
      glVertexAttribPointer(loc, vattr->size, vattr->type, GL_FALSE, vformat->stride, data + vattr->offset);
    }
  }
}

