// gpu_batch.c

typedef struct {
  uint8_t active;
  GLuint vaoId;   // ID for vertex array object
  GLuint vbuffId; // ID for vert buffer
  GLuint ibuffId; // ID for index buffer
} GPUBatch;

#define BATCH_MAX_COUNT 10

static GPUBatch batches[BATCH_MAX_COUNT];
static uint16_t next_deleted_batch = 0;

static GLuint find_deleted_batch(void)
{
  GLuint id = next_deleted_batch;
  
  if((id == 0) | (id >= BATCH_MAX_COUNT))
    id = 1;

  for ( ; id < INDEX_BUFFER_MAX_COUNT; id++) {
    if (batches[id].active == 0) {
      next_deleted_batch = id + 1;
      break;
    }
  }

  if (id == BATCH_MAX_COUNT) {
    printf("WARNING: No batches available\n");
    id = 0;
  }

  return id;
}

void GPU_batch_init(const GLuint batch_id)
{
  GPUBatch *batch = &batches[batch_id];
  
  batch->active = 1;
  
}

// create new batch
GLuint GPU_batch_create(void)
{
  const GLuint batch_id = find_deleted_batch();
  GPU_batch_init(batch_id);
  printf("New batch ID:%i\n", batch_id);

  return batch_id;
}


