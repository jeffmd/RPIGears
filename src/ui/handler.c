// handler.c

#include <stdio.h>
#include <stdint.h>

#include "static_array.h"
#include "action_table.h"

typedef struct {
  uint8_t active;
  short destination_id;
  short action_table;
} Handler;

#define HANDLER_MAX_COUNT 100

static Handler handlers[HANDLER_MAX_COUNT];
static short next_deleted_handler;

static inline int find_deleted_handler_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_handler, handlers, HANDLER_MAX_COUNT, Handler, "Handler");
}

static Handler *get_handler(int id)
{
  if ((id < 0) | (id >= HANDLER_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Handler id, using default id: 0\n");
  }
    
  return handlers + id;
}

static void handler_init(Handler *handler)
{

}

short Handler_create(const short destination_id, const short table_id)
{
  const int id = find_deleted_handler_id();
  Handler *const handler = get_handler(id);
  handler->active = 1;
  handler_init(handler);

  handler->destination_id = destination_id;
  handler->action_table = table_id;

  return id;
}

void Handler_execute(const short handler_id, const short slot_id, const short source_id)
{
  if (handler_id) {
    Handler *const handler = get_handler(handler_id);
    Action_table_execute(handler->action_table, slot_id, source_id, handler->destination_id);
  }
}

