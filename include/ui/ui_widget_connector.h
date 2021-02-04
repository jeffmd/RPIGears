// ui_widget_connector.h

#ifndef _UI_WIDGET_CONNECTOR_H_
  #define _UI_WIDGET_CONNECTOR_H_

  ID_t UI_widget_connector(const ID_t destination_class);
  void UI_widget_connect_changed(const ID_t connector_id, ActionFn action);
  void UI_widget_connect_update(const ID_t connector_id, ActionFn action);
  void UI_widget_changed(const Handle_t handle, const ID_t destination_id);
  void UI_widget_update(const Handle_t handle, const ID_t destination_id);

#endif