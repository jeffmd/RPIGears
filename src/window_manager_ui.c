// window_manager_ui.c

#include <stdio.h>
#include <stdint.h>

#include "window_manager.h"
#include "connector.h"
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

static void fps_update(const short source_id, const short destination_id)
{
  UI_number_update_float(source_id, WM_fps());
}

static short get_fps_ui_number_connector(void)
{
  if (!fps_ui_number_connector) {
    fps_ui_number_connector = UI_number_connector(get_window_manager_ui_class());
    //UI_number_connect_select(fps_ui_number_connector, fps_edit);
    UI_number_connect_update(fps_ui_number_connector, fps_update);
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
  }

  return fps_ui_number;
}

