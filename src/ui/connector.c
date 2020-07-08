// connector.c

#include <stdio.h>
#include <stdint.h>

#include "static_array.h"

typedef void (*ActionFn)(const short source_id, const short destination_id);

typedef struct {
  ActionFn action;
} ActionSlot;

typedef struct {
  uint8_t active;
  uint8_t count;
  short action_slot;
  short source_class;
  short destination_class;
} Connector;

typedef struct {
  const char *name;
} ClassReg;

typedef union {
  int ID;
  struct {
    short destination_id;
    short connector;
  };
} Handle;


#define CONNECTOR_MAX_COUNT 50
#define ACTION_SLOT_MAX_COUNT 1000

static ActionSlot action_slots[ACTION_SLOT_MAX_COUNT];
static short next_action_slot = 0;

static Connector connectors[CONNECTOR_MAX_COUNT];
static short next_deleted_connector;

#define CLASSREG_MAX_COUNT 50
static ClassReg classes[CLASSREG_MAX_COUNT];
static short next_class_id;

static short action_slot_allocate(const int count)
{
  const short new_action_slot = next_action_slot;

  next_action_slot += count;

  return new_action_slot;
}
 
static inline short find_deleted_connector(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_connector, connectors, CONNECTOR_MAX_COUNT, Connector, "Connector");
}

static Connector *get_connector(short id)
{
  if ((id < 0) | (id >= CONNECTOR_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad connector id, using default id: 0\n");
  }
    
  return connectors + id;
}

static void connector_init(Connector *connector)
{
  //action_table->action_slot = 0;
}

short Connector_create(const short source_class, const short destination_class)
{
  const short id = find_deleted_connector();
  Connector *const connector = get_connector(id);
  connector->active = 1;
  connector_init(connector);
  connector->source_class = source_class;
  connector->destination_class = destination_class;

  return id;
}

static void action_slots_init(const short id, const short count)
{
  const short last_id = id + count;
  for (short idx = id; idx < last_id; idx++) {
    action_slots[idx].action = 0;
  }
}

void Connector_allocate_slots(const short connector_id, const short count)
{
  Connector *const connector = get_connector(connector_id);
  
  connector->action_slot = action_slot_allocate(count);
  connector->count = count;

  action_slots_init(connector->action_slot, count);
}


void Connector_set_action(const short connector_id, const short slot_id, ActionFn action)
{
  Connector *const connector = get_connector(connector_id);

  if (slot_id < connector->count) {
    action_slots[connector->action_slot + slot_id].action = action;
  }
}

static void connector_execute(const short connector_id, const short slot_id, const short source_id, const short destination_id)
{
  Connector *const connector = get_connector(connector_id);

  if (slot_id < connector->count) {
    ActionSlot *slot = action_slots + connector->action_slot + slot_id;
    if (slot->action) {
      slot->action(source_id, destination_id);
    }
  }
}

short Connector_register_class(const char *name)
{
  next_class_id++;
  classes[next_class_id].name = name;

  return next_class_id;
}

int Connector_handle( const short connector, const short destination_id)
{
  Handle handle;

  handle.destination_id = destination_id;
  handle.connector = connector;

  return handle.ID;
}

void Connector_handle_execute(const int handle, const short slot_id, const short source_id)
{
  if (handle) {
    connector_execute(((Handle)handle).connector, slot_id, source_id, ((Handle)handle).destination_id);
  }
}


