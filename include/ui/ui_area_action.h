// ui_area_action.h - user interface area header

#ifndef _UI_AREA_ACTION_H_
  #define _UI_AREA_ACTION_H_

  short UI_area_create_action_table(void);
  void UI_area_action_set_attach(const short table_id, ActionFn action);
  void UI_area_action_set_enter(const short table_id, ActionFn action);
  void UI_area_action_set_leave(const short table_id, ActionFn action);
  void UI_area_action_set_draw(const short table_id, ActionFn action);
  void UI_area_action_set_resize(const short table_id, ActionFn action);
  void UI_area_action_set_move(const short table_id, ActionFn action);
  void UI_area_action_set_keyChange(const short table_id, ActionFn action);
  void UI_area_set_handler(const short area_id, const short handler_id);
  void UI_area_set_handled(const short area_id);
  void UI_area_draw(const short area_id);
  void UI_area_root_draw(void);
  void UI_area_key_change(const int key);
  int UI_area_active_key(void);

#endif