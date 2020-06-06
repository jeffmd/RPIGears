// action_table.h - 
#ifndef _ACTION_TABLE_H_
  #define _ACTION_TABLE_H_

  typedef void (*ActionFn)(const short source_id, const short destination_id);

  short Action_table_create(const short source_class, const short destination_class);
  void Action_table_allocate_slots(const short table_id, const short count);
  void Action_table_set_action(const short table_id, const short slot_id, ActionFn action);
  void Action_table_execute(const short table_id, const short slot_id, const short source_id, const short destination_id);
  short Action_table_new_class_id(void);

#endif