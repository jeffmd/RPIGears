// ui_widget_connector.h

#ifndef _UI_WIDGET_CONNECTOR_H_
  #define _UI_WIDGET_CONNECTOR_H_

  short UI_widget_connector(const short destination_class);
  void UI_widget_connect_change(const short connector_id, ActionFn action);
  void UI_widget_connect_update(const short connector_id, ActionFn action);
  void UI_widget_change(const int handle, const short destination_id);
  void UI_widget_update(const int handle, const short destination_id);

#endif