// ui_scene.c

#include <stdio.h>
#include <stdint.h>

#include "action_table.h"
#include "handler.h"
#include "ui_area.h"
#include "ui_area_action.h"

static short scene_area;
static short ver_text_area;
static short scene_action_table;
static short scene_handler;

static void ui_scene_enter(const short source_id, const short destination_id)
{
  printf("enter scene area\n");
}

static void ui_scene_leave(const short source_id, const short destination_id)
{
  printf("leave scene area\n");
}

static short ui_scene_get_action_table(void)
{
  if (!scene_action_table) {
    scene_action_table = UI_area_create_action_table();
    UI_area_action_set_enter(scene_action_table, ui_scene_enter);
    UI_area_action_set_leave(scene_action_table, ui_scene_leave);
  }

  return scene_action_table;
}

void UI_scene_create(void)
{
  scene_area = UI_area_create();
  UI_area_set_root(scene_area);
  UI_area_set_position(scene_area, 1, 1);
  UI_area_set_size(scene_area, 500, 500);

  ver_text_area =  UI_area_create();
  UI_area_set_size(ver_text_area, 50, 10);
  UI_area_set_position(ver_text_area, 30, 30);
  UI_area_add(scene_area, ver_text_area);

  scene_handler = Handler_create();
  Handler_set_source_destination(scene_handler, scene_area, 0);
  Handler_set_action_table(scene_handler, ui_scene_get_action_table());

  UI_area_set_handler(scene_area, scene_handler);
  
}