/*
* stats_ui.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "window_manager.h"
#include "key_input.h"
#include "text.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "ui_text.h"
#include "handler.h"

#define FPS_Y 400
#define FPS_X 60

static const char ver_text[] = "RPIGears ver: 1.0 GLES2.0";
static const char fps_text[] = "FPS:";
static const char num_text[] = "000.00";
static int fps_start;

static short fps_text_id; 
static short ver_area_id;
static short fps_area_id;
static short stats_area_id;
static short ver_ui_text_id;
static short fps_ui_text_id;

static short stats_draw;
static short stats_select_area_id;
static short stats_action_table;

static void update_stats_hide(void)
{
  UI_area_set_hide(stats_area_id, !stats_draw);
}

static void toggle_stats_draw(const short source_id, const short destination_id)
{
  stats_draw = !stats_draw;
  update_stats_hide();
  //UI_area_set_handled(source_id);
}

static void update_fps(const short source_id, const short destination_id)
{
  if (stats_draw) {
    const char *fps_str = WM_has_fps();

    if (fps_str) {
      Text_set_start(fps_text_id, fps_start);
      Text_add(fps_text_id, FPS_X, 0, fps_str);
    }
  }
}

static short get_stats_action_table(void)
{
  if (!stats_action_table) {
    stats_action_table = UI_area_create_action_table();
    UI_area_action_set_key_change(stats_action_table, toggle_stats_draw);
    UI_area_action_set_draw(stats_action_table, update_fps);
  }

  return stats_action_table;
}

static const int get_stats_area_handler(void)
{
  return Handler_create(0, get_stats_action_table());
}

static short get_ver_ui_text_id(void)
{
  if (!ver_ui_text_id) {
    ver_ui_text_id = UI_text_create();
    UI_text_add(ver_ui_text_id, ver_text);
  }

  return ver_ui_text_id;
}

static short get_ver_area_id(void)
{
  if (!ver_area_id) {
    ver_area_id = UI_area_create();
    UI_area_set_handler(ver_area_id, UI_text_area_handler(get_ver_ui_text_id()));
    UI_area_set_position(ver_area_id, 10, 10);
  }

  return ver_area_id;
}

static short get_fps_ui_text_id(void)
{
  if (!fps_ui_text_id) {
    fps_ui_text_id = UI_text_create();
    UI_text_add(fps_ui_text_id, fps_text);
    fps_text_id = UI_text_text_id(fps_ui_text_id);
    fps_start = Text_start(fps_text_id);
    Text_add(fps_ui_text_id, FPS_X, 0, num_text);
  }

  return fps_ui_text_id;
}

static short get_fps_area_id(void)
{
  if (!fps_area_id) {
    fps_area_id = UI_area_create();
    UI_area_set_handler(fps_area_id, UI_text_area_handler(get_fps_ui_text_id()));
    UI_area_set_position(fps_area_id, 10, FPS_Y);
  }

  return fps_area_id;
}

static short get_stats_area_id(void)
{
  if (!stats_area_id) {
    stats_area_id = UI_area_create();
    UI_area_set_position(stats_area_id, 2, 2);
    UI_area_set_size(stats_area_id, 300, 450);
    UI_area_add(stats_area_id, get_ver_area_id());
    UI_area_add(stats_area_id, get_fps_area_id());
    update_stats_hide();
  }

  return stats_area_id;
}

static short get_stats_select_area_id(void)
{
  if (!stats_select_area_id) {
    stats_select_area_id = UI_area_create();
    UI_area_set_position(stats_select_area_id, 0, 0);
    UI_area_set_size(stats_select_area_id, 10, 500);
    UI_area_set_handler(stats_select_area_id, get_stats_area_handler());

    Key_add_action(SHIFT_KEY('S'), toggle_stats_draw, "toggle stats render on/off");
  }

  return stats_select_area_id;
}

void Stats_ui_set_parent_area(const short parent_id)
{
  UI_area_add(parent_id, get_stats_area_id());
  UI_area_add(parent_id, get_stats_select_area_id());
}

