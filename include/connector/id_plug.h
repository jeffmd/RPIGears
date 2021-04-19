// typedefs.h

#ifndef _TYPEDEFS_H_
  #define _TYPEDEFS_H_

  typedef short ID_t;
  typedef int Plug_t;
  typedef void (*ActionFn)(const ID_t source_id, const ID_t destination_id);

#endif