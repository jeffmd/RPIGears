// key_action.h

#ifndef _KEY_MAP_H_
  #define _KEY_MAP_H_

  ID_t Key_Map_create(void);
  void Key_Map_add(const ID_t key_map_id, const ID_t key_action_id);
  void Key_Map_action(const ID_t key_map_id, const int key, const ID_t source_id,  const ID_t destination_id);
#endif