// ui_view3d.c

#include <stdio.h>
#include <stdint.h>

#include "connector.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "scene.h"

static short area_connector;
static short ui_view3d_class;

static void ui_view3d_enter(const short source_id, const short destination_id)
{
  printf("enter view3d area %i\n", source_id);
}

static void ui_view3d_leave(const short source_id, const short destination_id)
{
  printf("leave view3d area %i\n", source_id);
}

static void ui_view3d_draw(const short source_id, const short destination_id)
{
  Scene_draw();
}

static short get_class(void)
{
  if (!ui_view3d_class) {
    ui_view3d_class = Connector_register_class("ui_view3d");
  }

  return ui_view3d_class;
}

static short get_connector(void)
{
  if (!area_connector) {
    printf("create ui view3d area connector: ");
    area_connector = UI_area_connector(get_class());
    UI_area_connect_enter(area_connector, ui_view3d_enter);
    UI_area_connect_leave(area_connector, ui_view3d_leave);
    UI_area_connect_draw(area_connector, ui_view3d_draw);
  }

  return area_connector;
}

static int get_handle(const short ui_view3d_id)
{
  return Connector_handle(get_connector(), ui_view3d_id);
}

int UI_view3d_create(void)
{
  return get_handle(0);
}
