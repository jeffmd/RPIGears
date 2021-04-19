// key_action.c

#include <stdint.h>
#include <stdio.h>

#include "id_plug.h"
#include "static_array.h"

#define KEY_ACTION_MAX_COUNT 100

typedef void (*ActionFn)(const ID_t souce_id, const ID_t destination_id);

typedef struct {
  uint8_t active;
  int key;       // includes shift, ctrl, alt status in high bits
  ActionFn action;
  const char *help;
} KeyAction;

static KeyAction key_actions[KEY_ACTION_MAX_COUNT];
static ID_t next_deleted_key_action;

static inline ID_t find_deleted_key_action_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_key_action, key_actions, KEY_ACTION_MAX_COUNT, KeyAction, "key action");
}

static KeyAction *get_key_action(ID_t id)
{
  if ((id < 0) | (id >= KEY_ACTION_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad key action id, using default id: 0\n");
  }
    
  return key_actions + id;
}

static void key_action_init(KeyAction *key_action)
{

}

ID_t Key_Action_create(const int key, ActionFn action, const char* help)
{
  const ID_t id = find_deleted_key_action_id();
  KeyAction *const key_action = get_key_action(id);
  key_action->active = 1;
  key_action_init(key_action);

  key_action->key = key;
  key_action->action = action;
  key_action->help = help;

  return id;
}

int Key_Action_key(const ID_t key_action_id)
{
  return get_key_action(key_action_id)->key;
}

void Key_Action_execute(const ID_t key_action_id, const ID_t source_id,  const ID_t destination_id)
{
  get_key_action(key_action_id)->action(source_id, destination_id);
}

void Key_Action_print_help(void)
{
  for (int id = 0; id < KEY_ACTION_MAX_COUNT; id++) {
    KeyAction *key_action = get_key_action(id);
    if (key_action->active && key_action->help) {
      printf("%c - %s\n", (key_action->key & 0xff), key_action->help);
    }
  }  
}
