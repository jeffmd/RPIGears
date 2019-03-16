// gpu_vertex_format.h

#ifndef _GPU_VERTEX_FORMAT_H_
  #define _GPU_VERTEX_FORMAT_H_

  typedef struct GPUVertFormat GPUVertFormat;
  
  void GPU_vertex_format_init(GPUVertFormat *vformat);
  GPUVertFormat *GPU_vertex_format_create(void);
  void GPU_vertex_format_delete(GPUVertFormat *vformat);
  void GPU_vertex_format_add_attribute(GPUVertFormat *vformat, const char *name,
    const GLint size, const GLenum type);
  GLuint GPU_vertex_format_stride(GPUVertFormat *vformat);
  GLuint GPU_vertex_format_offset(GPUVertFormat *vformat, const GLuint idx);
  GLuint GPU_vertex_format_attribute_count(GPUVertFormat *vformat);
  void GPU_vertex_format_add_4(GPUVertFormat *vformat, const GLuint attribute_id,
    GLvoid *data, const GLfloat val1, const GLfloat val2, const GLfloat val3, const GLfloat val4);
  void GPU_vertex_format_bind(GPUVertFormat *vformat, GLvoid *data);
#endif
