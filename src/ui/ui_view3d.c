// ui_view3d.c

#include <stdio.h>
#include <stdint.h>

#include "action_table.h"
#include "handler.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "scene.h"

static short area_action_table;
static short area_handler;

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
  scene_draw();
}

static short get_action_table(void)
{
  if (!area_action_table) {
    area_action_table = UI_area_create_action_table();
    UI_area_action_set_enter(area_action_table, ui_view3d_enter);
    UI_area_action_set_leave(area_action_table, ui_view3d_leave);
    UI_area_action_set_draw(area_action_table, ui_view3d_draw);
  }

  return area_action_table;
}

void UI_view3d_create(void)
{

}

short UI_view3d_area_handler(void)
{
  if (!area_handler) {
    area_handler = Handler_create(0, get_action_table());
  }
  
  return area_handler;
}
