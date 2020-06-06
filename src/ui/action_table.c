// action_table.c

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
} ActionTable;

#define ACTION_TABLE_MAX_COUNT 50
#define ACTION_SLOT_MAX_COUNT 1000

static ActionSlot action_slots[ACTION_SLOT_MAX_COUNT];
static short next_action_slot = 0;

static ActionTable action_tables[ACTION_TABLE_MAX_COUNT];
static short next_deleted_action_table;

static short next_class_id;

static short action_slot_allocate(const int count)
{
  const short new_action_slot = next_action_slot;

  next_action_slot += count;

  return new_action_slot;
}
 
static inline int find_deleted_action_table_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_action_table, action_tables, ACTION_TABLE_MAX_COUNT, ActionTable, "Action Table");
}

static ActionTable *get_action_table(int id)
{
  if ((id < 0) | (id >= ACTION_TABLE_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Action table id, using default id: 0\n");
  }
    
  return action_tables + id;
}

static void action_table_init(ActionTable *action_table)
{
  //action_table->action_slot = 0;
}

int Action_table_create(const short source_class, const short destination_class)
{
  const int id = find_deleted_action_table_id();
  ActionTable *const action_table = get_action_table(id);
  action_table->active = 1;
  action_table_init(action_table);
  action_table->source_class = source_class;
  action_table->destination_class = destination_class;

  return id;
}

static void action_slots_init(const short id, const short count)
{
  const short last_id = id + count;
  for (short idx = id; idx < last_id; idx++) {
    action_slots[idx].action = 0;
  }
}

void Action_table_allocate_slots(const short table_id, const short count)
{
  ActionTable *const action_table = get_action_table(table_id);
  
  action_table->action_slot = action_slot_allocate(count);
  action_table->count = count;

  action_slots_init(action_table->action_slot, count);
}


void Action_table_set_action(const short table_id, const short slot_id, ActionFn action)
{
  ActionTable *const action_table = get_action_table(table_id);

  if (slot_id < action_table->count) {
    action_slots[action_table->action_slot + slot_id].action = action;
  }
}

void Action_table_execute(const short table_id, const short slot_id, const short source_id, const short destination_id)
{
  ActionTable *const action_table = get_action_table(table_id);

  if (slot_id < action_table->count) {
    ActionSlot *slot = action_slots + action_table->action_slot + slot_id;
    if (slot->action) {
      slot->action(source_id, destination_id);
    }
  }
}

short Action_table_new_class_id(void)
{
  return ++next_class_id;
}
