// gpu_vertex_format.c

#include <stdio.h>

#include "gles3.h"
#include "id_plug.h"
#include "static_array.h"
#include "gpu_shader.h"

typedef struct {
  GLenum type;             // GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_FIXED, or GL_FLOAT
  const char *name;        // attribute name in vertex shader
  GLuint offset;           // offset from start of data
  uint8_t size;            // Components per element (1,2,3,4)
  uint8_t normalized;      // Fixed-point values are normalized when converted to floats
} VertAttribute;

typedef struct {
  uint8_t active;           // not zero if vertex format is not deleted
  uint8_t attribute_count;  // count of active attributes in vertex_attributes array
  uint8_t stride;           // size in bytes of vertex data - only valid after attributes added and ready is not zero
  VertAttribute vertex_attributes[VERT_ATTRIB_MAX];
} GPUVertFormat;

#define VERT_FORMAT_MAX_COUNT 5

static GPUVertFormat vert_formats[VERT_FORMAT_MAX_COUNT];
static ID_t next_deleted_vert_format;

static inline ID_t find_deleted_vert_format_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_vert_format, vert_formats,
                            VERT_FORMAT_MAX_COUNT, GPUVertFormat, "vertex format");
}

static GPUVertFormat *get_vert_format(ID_t id)
{
  if ((id < 0) | (id >= VERT_FORMAT_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Vertex Format id, using default id: 0\n");
  }
    
  return vert_formats + id;
}

static void GPU_vertex_format_init(GPUVertFormat *vformat)
{
  vformat->attribute_count = 0;
  vformat->stride = 0;
}

ID_t GPU_vertex_format_create(void)
{
  const ID_t id = find_deleted_vert_format_id();
  GPUVertFormat *const vformat = get_vert_format(id);

  vformat->active = 1;
  GPU_vertex_format_init(vformat);

  return id;
}


// add attribute to vertex format
void GPU_vertex_format_add_attribute(const ID_t id, const char *name, const GLint size, const GLenum type)
{
  GPUVertFormat *const vformat = get_vert_format(id);
  VertAttribute *const vattr = &vformat->vertex_attributes[vformat->attribute_count];
  
  vattr->type = type;
  vattr->size = size;
  vattr->normalized = GL_FALSE;
  vattr->name = name;
                               
  vformat->attribute_count++;
  vformat->stride = 0;
}

void GPU_vertex_format_delete(const ID_t id)
{
  GPUVertFormat *const vformat = get_vert_format(id);

  vformat->active = 0;
  
  if (id < next_deleted_vert_format)
    next_deleted_vert_format = id; 
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

GLuint GPU_vertex_format_stride(const ID_t id)
{
  GPUVertFormat *const vformat = get_vert_format(id);

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

GLuint GPU_vertex_format_offset(const ID_t id, const GLuint idx)
{
  return get_vert_format(id)->vertex_attributes[idx].offset;
}

GLuint GPU_vertex_format_attribute_count(const ID_t id)
{
  return get_vert_format(id)->attribute_count;
}

void GPU_vertex_format_add_4(const ID_t id, const GLuint attribute_id, GLvoid *attr_data, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4)
{
  GPUVertFormat *const vformat = get_vert_format(id);
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
      __fp16 *data = attr_data;
      data[0] = val1;
      if (size > 1)
        data[1] = val2;
      if (size > 2)
        data[2] = val3;
      if (size > 3)
        data[3] = val4;
    }
  }
  else {
    printf("ERROR: vertex format cannot add values to buffer, buffer pointer is NULL\n");
  }
}

void GPU_vertex_format_read_data(const ID_t id, const GLuint attribute_id, GLvoid *attr_data, GLfloat val[4])
{
  GPUVertFormat *const vformat = get_vert_format(id);
  if (attr_data) {
    VertAttribute *vattr = &vformat->vertex_attributes[attribute_id];
    const GLuint size = vattr->size;
    
    if (vattr->type == GL_FLOAT) {
      GLfloat *data = attr_data;
      val[0] = data[0];
      
      if (size > 1)
        val[1] = data[1];
      if (size > 2)
        val[2] = data[2];
      if (size > 3)
        val[3] = data[3];
    }
    else if (vattr->type == GL_HALF_FLOAT_OES) {
      __fp16 *data = attr_data;
      val[0] = data[0];

      if (size > 1)
        val[1] = data[1];
      if (size > 2)
        val[2] = data[2];
      if (size > 3)
        val[3] = data[3];
    }
  }
  else {
    printf("ERROR: vertex format cannot add values to buffer, buffer pointer is NULL\n");
  }
}

void GPU_vertex_format_bind(const ID_t id, GLvoid *data)
{
  GPUVertFormat *const vformat = get_vert_format(id);

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

