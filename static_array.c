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

