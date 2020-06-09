// ui_area_action.h - user interface area header

#ifndef _UI_AREA_ACTION_H_
  #define _UI_AREA_ACTION_H_

  short UI_area_connector(const short destination_class);
  void UI_area_connect_attach(const short connector_id, ActionFn action);
  void UI_area_connect_enter(const short connector_id, ActionFn action);
  void UI_area_connect_leave(const short connector_id, ActionFn action);
  void UI_area_connect_draw(const short connector_id, ActionFn action);
  void UI_area_connect_resize(const short connector_id, ActionFn action);
  void UI_area_connect_move(const short connector_id, ActionFn action);
  void UI_area_connect_key_change(const short connector_id, ActionFn action);
  void UI_area_connect(const short area_id, const int handle);
  void UI_area_set_handled(const short area_id);
  void UI_area_select_active(const int x, const int y);
  void UI_area_draw(const short area_id);
  void UI_area_root_draw(void);
  void UI_area_key_change(const int key);
  int UI_area_active_key(void);

#endif