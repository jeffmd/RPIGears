// handler.c

#include <stdio.h>
#include <stdint.h>

#include "connector.h"

typedef union {
  int ID;
  struct {
    short destination_id;
    short connector;
  };
} Handler;


int Handler_create(const short destination_id, const short connector)
{
  Handler handler;

  handler.destination_id = destination_id;
  handler.connector = connector;

  return handler.ID;
}

void Handler_execute(const int handler_id, const short slot_id, const short source_id)
{
  if (handler_id) {
    Handler handler = {.ID = handler_id};
    Connector_execute(handler.connector, slot_id, source_id, handler.destination_id);
  }
}

