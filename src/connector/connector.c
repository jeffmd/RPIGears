// connector.c

#include <stdio.h>
#include <stdint.h>

#include "id_plug.h"
#include "static_array.h"

typedef struct {
  uint8_t active;
  uint8_t pin_count;
  ID_t action_start;
  ID_t source_class;
  ID_t destination_class;
} Connector;

typedef struct {
  const char *name;
} ClassReg;

typedef union {
  Plug_t id;
  struct {
    ID_t destination_id;
    ID_t connector;
  };
} Plug;


#define CONNECTOR_MAX_COUNT 50
#define ACTION_MAX_COUNT 1000

static ActionFn actions[ACTION_MAX_COUNT];
static ID_t next_action;

static Connector connectors[CONNECTOR_MAX_COUNT];
static ID_t next_deleted_connector;

#define CLASSREG_MAX_COUNT 50
static ClassReg classes[CLASSREG_MAX_COUNT];
static ID_t next_class_id;

static ID_t actions_allocate(const int count)
{
  const ID_t action_start = next_action;

  next_action += count;

  return action_start;
}
 
static inline ID_t find_deleted_connector(void)
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

static void actions_init(const ID_t id, const short count)
{
  const ID_t last_id = id + count;
  for (ID_t idx = id; idx < last_id; idx++) {
    actions[idx] = 0;
  }
}

static void allocate_actions(Connector *const connector, const short pin_count)
{
  connector->action_start = actions_allocate(pin_count);
  connector->pin_count = pin_count;

  actions_init(connector->action_start, pin_count);
}


ID_t Connector_create(const ID_t source_class, const ID_t destination_class, const short pin_count)
{
  const ID_t id = find_deleted_connector();
  Connector *const connector = get_connector(id);
  connector->active = 1;
  connector_init(connector);
  connector->source_class = source_class;
  connector->destination_class = destination_class;

  allocate_actions(connector, pin_count);

  return id;
}

void Connector_set_pin_action(const ID_t connector_id, const ID_t pin_id, ActionFn action)
{
  Connector *const connector = get_connector(connector_id);

  if (pin_id < connector->pin_count) {
    actions[connector->action_start + pin_id] = action;
  }
}

ID_t Connector_register_class(const char *name)
{
  next_class_id++;
  classes[next_class_id].name = name;

  return next_class_id;
}

Plug_t Connector_plug( const ID_t connector, const ID_t destination_id)
{
  const Plug plug = {.destination_id = destination_id, .connector = connector};

  return plug.id;
}

void Connector_plug_execute(const Plug_t plug, const ID_t pin_id, const ID_t source_id)
{
  if (plug) {
    Connector *const connector = get_connector(((Plug)plug).connector);

    if (pin_id < connector->pin_count) {
      ActionFn action = actions[connector->action_start + pin_id];
      if (action) {
        action(source_id, ((Plug)plug).destination_id);
      }
    }
  }
}

