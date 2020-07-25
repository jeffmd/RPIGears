/*
* stats_ui.c
*/

#include <stdio.h>
#include <stdint.h>

#include "window_manager.h"
#include "key_input.h"
#include "text.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "ui_text.h"
#include "ui_checkbox.h"
#include "connector.h"

#define FPS_Y 400
#define FPS_X 60

static const char ver_str[] = "RPIGears ver: 1.0 GLES2.0";
static const char fps_str[] = "FPS:";
static const char num_str[] = "000.000";
static const char vsync_str[] = "vsync";

static int fps_start;
static int fps_ui_text;
static int ver_ui_text;
static int vsync_ui_checkbox;


static short fps_text; 

static short stats_draw;
static short stats_area;
static short stats_select_area;
static short stats_connector;
static short stats_class;

static void update_stats_hide(void)
{
  UI_area_set_hide(stats_area, !stats_draw);
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
      Text_set_index(fps_text, fps_start);
      Text_add(fps_text, FPS_X, 0, fps_str);
    }
  }
}

static short get_stats_class(void)
{
  if (!stats_class) {
    stats_class = Connector_register_class("stats_ui");
  }

  return stats_class;
}

static short get_stats_connector(void)
{
  if (!stats_connector) {
    stats_connector = UI_area_connector(get_stats_class());
    UI_area_connect_key_change(stats_connector, toggle_stats_draw);
    UI_area_connect_draw(stats_connector, update_fps);
  }

  return stats_connector;
}

static const int get_stats_handle(void)
{
  return Connector_handle(get_stats_connector(), 0);
}

static int get_ver_ui_text(void)
{
  if (!ver_ui_text) {
    ver_ui_text = UI_text_create(ver_str);
  }

  return ver_ui_text;
}

static int get_vsync_ui_checkbox(void)
{
  if (!vsync_ui_checkbox) {
    vsync_ui_checkbox = UI_checkbox_create(vsync_str);
  }

  return vsync_ui_checkbox;
}

static int get_fps_ui_text(void)
{
  if (!fps_ui_text) {
    fps_ui_text = UI_text_create(fps_str);
    fps_text = UI_text_text_id(fps_ui_text);
    fps_start = Text_index(fps_text);
    Text_add(fps_text, FPS_X, 0, num_str);
  }

  return fps_ui_text;
}


static short get_stats_area(void)
{
  if (!stats_area) {
    stats_area = UI_area_create();
    UI_area_set_position(stats_area, 2, 2);
    UI_area_set_size(stats_area, 300, 450);
    UI_area_add_handle(stats_area, get_ver_ui_text(), 10, 10);
    UI_area_add_handle(stats_area, get_fps_ui_text(), 10, FPS_Y);
    UI_area_add_handle(stats_area, get_vsync_ui_checkbox(), 10, 30);
    update_stats_hide();
  }

  return stats_area;
}

static short get_stats_select_area(void)
{
  if (!stats_select_area) {
    stats_select_area = UI_area_create();
    UI_area_set_position(stats_select_area, 0, 0);
    UI_area_set_size(stats_select_area, 10, 500);
    UI_area_connect(stats_select_area, get_stats_handle());

    Key_add_action(SHIFT_KEY('S'), toggle_stats_draw, "toggle stats render on/off");
  }

  return stats_select_area;
}

void Stats_ui_set_parent_area(const short parent_area)
{
  UI_area_add(parent_area, get_stats_area());
  UI_area_add(parent_area, get_stats_select_area());
}

