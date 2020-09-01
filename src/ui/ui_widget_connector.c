// ui_widget_connector.c

#include <stdio.h>
#include <stdint.h>

#include "connector.h"

enum Events {
  OnChange,
  OnUpdate,
  EventsMax
};

static short ui_widget_class;

static short get_ui_widget_class(void)
{
  if(!ui_widget_class) {
    ui_widget_class = Connector_register_class("ui_widget");
  }

  return ui_widget_class;
}

short UI_widget_connector(const short destination_class)
{
  const short connector_id = Connector_create(get_ui_widget_class(), destination_class, EventsMax);

  return connector_id;
}

void UI_widget_connect_change(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnChange, action);
}

void UI_widget_connect_update(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnUpdate, action);
}

void UI_widget_change(const int handle, const short destination_id)
{
  Connector_handle_execute(handle, OnChange, destination_id);
}

void UI_widget_update(const int handle, const short destination_id)
{
  Connector_handle_execute(handle, OnUpdate, destination_id);
}

