// static_array.h

#ifndef _STATIC_ARRAY_H_
  #define _STATIC_ARRAY_H_
  
  int array_find_deleted_id(int *next_deleted, void *array, const int maxId, const size_t size, const char *object_name);

  #define ARRAY_FIND_DELETED_ID(next_deleted, array, maxcount, array_struct, name) \
    array_find_deleted_id(&next_deleted, array, maxcount, sizeof(array_struct), name)
#endif
