// ui_view3d.c

#include <stdio.h>
#include <stdint.h>

#include "action_table.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "scene.h"

static short view3d_area;
static short view3d_action_table;

static void ui_view3d_enter(const short source_id, const short destination_id)
{
  printf("enter area %i\n", source_id);
}

static void ui_view3d_leave(const short source_id, const short destination_id)
{
  printf("leave area %i\n", source_id);
}

static void ui_view3d_draw(const short source_id, const short destination_id)
{
  scene_draw();
}

static short ui_view3d_get_action_table(void)
{
  if (!view3d_action_table) {
    view3d_action_table = UI_area_create_action_table();
    UI_area_action_set_enter(view3d_action_table, ui_view3d_enter);
    UI_area_action_set_leave(view3d_action_table, ui_view3d_leave);
    UI_area_action_set_draw(view3d_action_table, ui_view3d_draw);
  }

  return view3d_action_table;
}

void UI_view3d_create(void)
{
  view3d_area = UI_area_create();
  UI_area_set_root(view3d_area);
  UI_area_set_position(view3d_area, 1, 1);
  UI_area_set_size(view3d_area, 600, 500);

  UI_area_set_handler(view3d_area, 0, ui_view3d_get_action_table());
  
}