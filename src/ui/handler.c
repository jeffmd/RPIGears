// handler.c

#include <stdio.h>
#include <stdint.h>

#include "static_array.h"
#include "action_table.h"

typedef struct Handler {
  uint8_t active;
  short source_id;
  short destination_id;
  short action_table;
} Handler;

#define HANDLER_MAX_COUNT 100

static Handler handlers[HANDLER_MAX_COUNT];
static short next_deleted_handler;

static inline int find_deleted_handler_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_handler, handlers, HANDLER_MAX_COUNT, Handler, "Action Table");
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
  //action_table->action_slot = 0;
}

int Handler_create(void)
{
  const int id = find_deleted_handler_id();
  Handler *const handler = get_handler(id);
  handler->active = 1;
  handler_init(handler);

  return id;
}

void Handler_set_source_destination(const short handler_id, const short source_id, const short destination_id)
{
  Handler *const handler = get_handler(handler_id);

  handler->source_id = source_id;
  handler->destination_id = destination_id;
}

void Handler_set_action_table(const short handler_id, const short table_id)
{
  Handler *const handler = get_handler(handler_id);

  handler->action_table = table_id;
}

void Handler_execute(const short handler_id, const short slot_id)
{
  Handler *const handler = get_handler(handler_id);
  Action_table_execute(handler->action_table, slot_id, handler->source_id, handler->destination_id);
}

