// key_action.h

#ifndef _KEY_MAP_H_
  #define _KEY_MAP_H_

  short Key_Map_create(void);
  void Key_Map_add(const short key_map_id, const short key_action_id);
  void Key_Map_action(const short key_map_id, const short key, const short source_id,  const short destination_id);
#endif