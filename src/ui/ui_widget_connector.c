// ui_widget_connector.c

#include <stdio.h>
#include <stdint.h>

#include "static_array.h"
#include "connector.h"

enum Events {
  OnChange,
  OnUpdate,
  EventsMax
};

static ID_t ui_widget_class;

static ID_t get_ui_widget_class(void)
{
  if(!ui_widget_class) {
    ui_widget_class = Connector_register_class("ui_widget");
  }

  return ui_widget_class;
}

ID_t UI_widget_connector(const ID_t destination_class)
{
  const ID_t connector_id = Connector_create(get_ui_widget_class(), destination_class, EventsMax);

  return connector_id;
}

void UI_widget_connect_changed(const ID_t connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnChange, action);
}

void UI_widget_connect_update(const ID_t connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnUpdate, action);
}

void UI_widget_changed(const Handle_t handle, const ID_t destination_id)
{
  Connector_handle_execute(handle, OnChange, destination_id);
}

void UI_widget_update(const Handle_t handle, const ID_t destination_id)
{
  Connector_handle_execute(handle, OnUpdate, destination_id);
}

