// static_array.h

#ifndef _STATIC_ARRAY_H_
  #define _STATIC_ARRAY_H_
  
  void *array_find_deleted(void **next_deleted, const void *start, const void *end, const size_t size, const char *object_name);

  #define ARRAY_FIND_DELETED(next_deleted, start, count, name) \
    array_find_deleted((void **)&next_deleted, start, start + count, sizeof(start) / count, name)
#endif
