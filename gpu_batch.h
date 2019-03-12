// gpu_batch.h

#ifndef _GPU_BATCH_H_
  #define _GPU_BATCH_H_
  
void GPU_batch_init(const GLuint batch_id);
GLuint GPU_batch_create(void);
void GPU_batch_delete(const GLuint batch_id, const int delete_buffers);
void GPU_batch_set_indices_draw_count(const GLuint batch_id, const int count);
void GPU_batch_set_vertices_draw_count(const GLuint batch_id, const int count);
void GPU_batch_draw(const GLuint batch_id, const GLenum drawMode, const GLuint instances);
void GPU_batch_set_vertex_buffer(const GLuint batch_id, const GLuint vbuffId);
void GPU_batch_set_index_buffer(const GLuint batch_id, const GLuint ibuffId);
void GPU_batch_set_VAO(const GLuint batch_id);
  
#endif
