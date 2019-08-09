// static_array.c

#include <stdio.h>

void *array_find_deleted(void **next_deleted, void *start, const void *end, size_t size, const char *object_name)
{
  void *deleted = *next_deleted;

  if ((deleted <= start) | (deleted >= end))
    deleted = start + size;

  for ( ; deleted < end; deleted += size) {
    if (*(char *)deleted == 0) {
      *next_deleted = deleted + size;
      break;
    }
  }
  
  if (deleted >= end) {
    printf("WARNING: No %s available\n", object_name);
    deleted = start;
  }
  
  printf("%s ID: %i\n", object_name, (deleted - start)  / size);
  return deleted;
}

int array_find_deleted_id(int *next_deleted, void *array, const int maxId, const size_t size, const char *object_name)
{
  int id = *next_deleted;

	if ((id <= 0) | (id >= maxId))
	  id = 1;

  for ( ; id < maxId; id++) {
    const char active = *(char *)(array + (id * size)); 
    if (active == 0) {
      *next_deleted = id + 1;
      break;
    }
  }
  
  if (id >= maxId) {
    printf("WARNING: No %s available\n", object_name);
    id = 0;
  }
  
  printf("%s ID: %i\n", object_name, id);
  
  return id;
}

