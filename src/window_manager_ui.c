// window_manager_ui.c

#include <stdio.h>
#include <stdint.h>

#include "window_manager.h"
#include "connector.h"
#include "ui_widget_connector.h"
#include "ui_number.h"

static const char fps_str[] = "FPS";
static int fps_ui_number;
static short fps_ui_number_connector;
static short window_manager_ui_class;

static short get_window_manager_ui_class(void)
{
  if (!window_manager_ui_class) {
    window_manager_ui_class = Connector_register_class("stats_ui");
  }

  return window_manager_ui_class;
}

static void fps_update_ui(const short source_id, const short destination_id)
{
  UI_number_update_float(source_id, WM_fps());
}

static void fps_ui_changed(const short source_id, const short destination_id)
{
  WM_set_fps(UI_number_float_new_val(source_id));
}

static short get_fps_ui_number_connector(void)
{
  if (!fps_ui_number_connector) {
    fps_ui_number_connector = UI_widget_connector(get_window_manager_ui_class());
    UI_widget_connect_changed(fps_ui_number_connector, fps_ui_changed);
    UI_widget_connect_update(fps_ui_number_connector, fps_update_ui);
  }

  return fps_ui_number_connector;
}

static const int get_fps_ui_number_handle(void)
{
  return Connector_handle(get_fps_ui_number_connector(), 0);
}

int WM_ui_fps(void)
{
  if (!fps_ui_number) {
    fps_ui_number = UI_number_create(fps_str, get_fps_ui_number_handle());
    UI_number_set_edit(fps_ui_number, 1);
    UI_number_set_default_float_change(fps_ui_number, 1.0f);
  }

  return fps_ui_number;
}

