// handler.c

#include <stdio.h>
#include <stdint.h>

#include "action_table.h"

typedef union {
  int ID;
  struct {
    short destination_id;
    short action_table;
  };
} Handler;


int Handler_create(const short destination_id, const short table_id)
{
  Handler handler;

  handler.destination_id = destination_id;
  handler.action_table = table_id;

  return handler.ID;
}

void Handler_execute(const int handler_id, const short slot_id, const short source_id)
{
  if (handler_id) {
    Handler handler = {.ID = handler_id};
    Action_table_execute(handler.action_table, slot_id, source_id, handler.destination_id);
  }
}

