// ui_area_action.h - user interface area header

#ifndef _UI_AREA_ACTION_H_
  #define _UI_AREA_ACTION_H_

  ID_t UI_area_connector(const ID_t destination_class);
  void UI_area_connect_attach(const ID_t connector_id, ActionFn action);
  void UI_area_connect_enter(const ID_t connector_id, ActionFn action);
  void UI_area_connect_leave(const ID_t connector_id, ActionFn action);
  void UI_area_connect_draw(const ID_t connector_id, ActionFn action);
  void UI_area_connect_resize(const ID_t connector_id, ActionFn action);
  void UI_area_connect_move(const ID_t connector_id, ActionFn action);
  void UI_area_connect_key_change(const ID_t connector_id, ActionFn action);
  void UI_area_connect_pointer_move(const ID_t connector_id, ActionFn action);
  void UI_area_connect_pointer_drag(const ID_t connector_id, ActionFn action);
  void UI_area_connect(const ID_t area_id, const Plug_t plug);
  ID_t UI_area_add_plug(const ID_t parent_id, const Plug_t plug, const int x, const int y);
  void UI_area_set_handled(const ID_t area_id);
  int UI_area_handled(const ID_t area_id);
  void UI_area_select_active(const int key, const int x, const int y);
  int UI_area_locked_hit(void);
  void UI_area_draw(const ID_t area_id);
  void UI_area_root_draw(void);
  void UI_area_key_change(const int key);
  int UI_area_active_key(void);
  void UI_area_set_locked(const ID_t area_id);
  void UI_area_set_unlocked(const ID_t area_id);
  void UI_area_drag_start(void);
  void UI_area_drag_end(void);
  int UI_area_dragging(void);
  int UI_area_drag_delta_xy(void);
  int UI_area_drag_delta_y(void);
  int UI_area_drag_delta_x(void);

#endif