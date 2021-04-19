// key_map.c

#include <stdint.h>
#include <stdio.h>

#include "id_plug.h"
#include "static_array.h"
#include "key_action.h"

#define KEY_MAP_MAX_COUNT 10
#define KEY_LINK_MAX_COUNT 200
#define LINK_COUNT 32
#define LINK_MOD (LINK_COUNT - 1)

typedef struct {
  ID_t key_action_id;
  ID_t next;
} KeyLink;

typedef struct {
  uint8_t active;
  ID_t links[LINK_COUNT];
} KeyMap;

static KeyMap  key_maps[KEY_MAP_MAX_COUNT];
static ID_t next_deleted_key_map;

static KeyLink key_links[KEY_LINK_MAX_COUNT];
static ID_t next_deleted_key_link;

static uint8_t get_link_id(const uint8_t key)
{
  return (key & LINK_MOD);
}

static ID_t key_link_create(void)
{
  if (next_deleted_key_link < (KEY_LINK_MAX_COUNT - 1))
    return ++next_deleted_key_link;

  printf("no key links left\n");
  return 0;
}

static KeyLink *get_key_link(const ID_t key_link_id)
{
  return key_links + key_link_id;
}

static inline ID_t find_deleted_key_map_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_key_map, key_maps, KEY_MAP_MAX_COUNT, KeyMap, "key map");
}

static KeyMap *get_key_map(ID_t id)
{
  if ((id < 0) | (id >= KEY_MAP_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad key map id, using default id: 0\n");
  }
    
  return key_maps + id;
}

static void key_map_init(KeyMap *key_action)
{

}

ID_t Key_Map_create(void)
{
  const ID_t id = find_deleted_key_map_id();
  KeyMap *const key_map = get_key_map(id);
  key_map->active = 1;
  key_map_init(key_map);

  return id;
}

void Key_Map_add(const ID_t key_map_id, const ID_t key_action_id)
{
  KeyMap *key_map = get_key_map(key_map_id); 
  const uint8_t link_idx = get_link_id(Key_Action_key(key_action_id));

  const ID_t key_link_id = key_link_create();
  KeyLink *key_link = get_key_link(key_link_id);
  key_link->key_action_id = key_action_id;
  key_link->next = key_map->links[link_idx];
  key_map->links[link_idx] = key_link_id;  
}

void Key_Map_action(const ID_t key_map_id, const int key, const ID_t source_id,  const ID_t destination_id)
{
  KeyMap *key_map = get_key_map(key_map_id); 
  const uint8_t link_idx = get_link_id(key);

  ID_t key_link_id = key_map->links[link_idx];

  while (key_link_id) {
    KeyLink *key_link = get_key_link(key_link_id);
    if (Key_Action_key(key_link->key_action_id) == key) {
      Key_Action_execute(key_link->key_action_id, source_id, destination_id);
      key_link_id = 0;
    }
    else {
      key_link_id = key_link->next;
    }
  }
}

