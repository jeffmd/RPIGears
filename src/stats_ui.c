/*
* stats_ui.c
*/

#include <stdio.h>
#include <stdint.h>

#include "static_array.h"
#include "connector.h"
#include "key_input.h"
#include "ui_area.h"
#include "ui_layout.h"
#include "ui_area_action.h"
#include "ui_text.h"
#include "user_options_ui.h"
#include "demo_state_ui.h"
#include "window_manager_ui.h"
#include "ui_slider.h"
#include "ui_button.h"

static const char ver_str[] = "RPIGears ver: 1.0 GLES2.0";
static Handle_t ver_ui_text;

static ID_t stats_draw;
static ID_t stats_area;
static ID_t stats_layout;
static ID_t stats_select_area;
static ID_t stats_area_connector;
//static short stats_checkbox_connector;
static ID_t stats_class;

static void update_stats_hide(void)
{
  UI_area_set_hide(stats_area, !stats_draw);
}

static void toggle_stats_draw(const ID_t source_id, const ID_t destination_id)
{
  stats_draw = !stats_draw;
  update_stats_hide();
  //UI_area_set_handled(source_id);
}

static ID_t get_stats_class(void)
{
  if (!stats_class) {
    stats_class = Connector_register_class("stats_ui");
  }

  return stats_class;
}

static ID_t get_stats_area_connector(void)
{
  if (!stats_area_connector) {
    stats_area_connector = UI_area_connector(get_stats_class());
    UI_area_connect_key_change(stats_area_connector, toggle_stats_draw);
    //UI_area_connect_draw(stats_area_connector, update_fps);
  }

  return stats_area_connector;
}

static const Handle_t get_stats_area_handle(void)
{
  return Connector_handle(get_stats_area_connector(), 0);
}

static Handle_t get_ver_ui_text(void)
{
  if (!ver_ui_text) {
    ver_ui_text = UI_text_create(ver_str);
  }

  return ver_ui_text;
}

static ID_t get_stats_area(void)
{
  if (!stats_area) {
    stats_area = UI_area_create();
    UI_area_set_offset(stats_area, 10, 2);
    UI_area_set_size(stats_area, 300, 450);
    stats_layout = UI_Layout_create();
    UI_area_set_layout(stats_area, stats_layout);

    UI_area_add_handle(stats_area, get_ver_ui_text(), 0, 0);
    UI_area_add_handle(stats_area, User_options_ui_vsync(), 0, 0);
    UI_area_add_handle(stats_area, DS_ui_vbo(), 0, 0);
    UI_area_add_handle(stats_area, DS_ui_instances(), 0, 0);
    UI_area_add_handle(stats_area, DS_ui_rpm(), 0, 0);
    UI_area_add_handle(stats_area, WM_ui_fps(), 0, 40);
    update_stats_hide();
  }

  return stats_area;
}

static ID_t get_stats_select_area(void)
{
  if (!stats_select_area) {
    stats_select_area = UI_area_create();
    UI_area_set_offset(stats_select_area, 0, 0);
    UI_area_set_size(stats_select_area, 10, 500);
    UI_area_connect(stats_select_area, get_stats_area_handle());

    Key_add_action(SHIFT_KEY('S'), toggle_stats_draw, "toggle stats render on/off");
  }

  return stats_select_area;
}

void Stats_ui_set_parent_area(const ID_t parent_area)
{
  UI_area_add(parent_area, get_stats_area());
  UI_area_add(parent_area, get_stats_select_area());
  UI_area_add_handle(parent_area, UI_slider_create(0), 350, 20);
  UI_area_add_handle(parent_area, UI_button_create("t1", 0), 350, 70);
  UI_area_add_handle(parent_area, UI_button_create("t2", 0), 350, 100);
}

