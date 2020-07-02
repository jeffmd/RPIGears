// static_array.c

#include <stdio.h>

int array_find_deleted_id(short *next_deleted, void *array, const short maxId, const size_t size, const char *object_name)
{
  short id = *next_deleted;

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

