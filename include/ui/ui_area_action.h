// ui_area_action.h - user interface area header

#ifndef _UI_AREA_ACTION_H_
  #define _UI_AREA_ACTION_H_

  int UI_area_create_action_table(void);
  void UI_area_action_set_enter(const short table_id, ActionFn action);
  void UI_area_action_set_leave(const short table_id, ActionFn action);
  void UI_area_set_handler(const short area_id, const short handler_id);

#endif