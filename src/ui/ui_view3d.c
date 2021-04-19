// ui_view3d.c

#include <stdio.h>
#include <stdint.h>

#include "id_plug.h"
#include "connector.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "scene.h"

static ID_t area_connector;
static ID_t ui_view3d_class;

/*
static void ui_view3d_enter(const ID_t source_id, const ID_t destination_id)
{
  //printf("enter view3d area %i\n", source_id);
}

static void ui_view3d_leave(const ID_t source_id, const ID_t destination_id)
{
  //printf("leave view3d area %i\n", source_id);
}
*/

static void ui_view3d_draw(const ID_t source_id, const ID_t destination_id)
{
  Scene_draw();
}

static ID_t get_ui_view3d_class(void)
{
  if (!ui_view3d_class) {
    ui_view3d_class = Connector_register_class("ui_view3d");
  }

  return ui_view3d_class;
}

static ID_t get_area_connector(void)
{
  if (!area_connector) {
    printf("create ui view3d area connector: ");
    area_connector = UI_area_connector(get_ui_view3d_class());
    //UI_area_connect_enter(area_connector, ui_view3d_enter);
    //UI_area_connect_leave(area_connector, ui_view3d_leave);
    UI_area_connect_draw(area_connector, ui_view3d_draw);
  }

  return area_connector;
}

static Plug_t get_plug(const ID_t ui_view3d_id)
{
  return Connector_plug(get_area_connector(), ui_view3d_id);
}

Plug_t UI_view3d_create(void)
{
  return get_plug(0);
}
