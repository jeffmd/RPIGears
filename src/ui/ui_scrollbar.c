// ui_scrollbar.c

#include <stdio.h>
#include <stdint.h>

#include "key_map.h"
#include "key_action.h"
#include "key_input.h"

#include "connector.h"
#include "ui_widget_connector.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "static_array.h"
#include "ui_icon.h"

typedef struct {
  uint8_t active;
  uint8_t scrolling:1;
  short scroll_area;
  short area;
  float select_scale[2];
  float select_offset[2];
} UI_Scrollbar;

#define UI_SCROLLBAR_MAX_COUNT 50

static UI_Scrollbar ui_scrollbars[UI_SCROLLBAR_MAX_COUNT];
static short next_deleted_ui_scrollbar;

static short area_connector;
static short ui_scrollbar_class;
static short ui_scrollbar_key_map;

static inline short find_deleted_ui_scrollbar(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_ui_scrollbar, ui_scrollbars, UI_SCROLLBAR_MAX_COUNT, UI_Scrollbar, "UI scrollbar");
}

static UI_Scrollbar *get_ui_scrollbar(short id)
{
  if ((id < 0) | (id >= UI_SCROLLBAR_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI scrollbar id, using default id: 0\n");
  }
    
  return ui_scrollbars + id;
}

static void ui_scrollbar_init(UI_Scrollbar *ui_scrollbar)
{

}

static short get_ui_scrollbar_class(void)
{
  if(!ui_scrollbar_class) {
    ui_scrollbar_class = Connector_register_class("ui_scrollbar");
  }

  return ui_scrollbar_class;
}

static void update_dimensions(UI_Scrollbar *ui_scrollbar, const short area_id)
{
  if (ui_scrollbar->area != area_id)
  {
    short size[2];

    ui_scrollbar->area = area_id;
    UI_area_size(area_id, size);
    ui_scrollbar->select_scale[0] = 2.0f * size[0] - 2.0f;
    ui_scrollbar->select_scale[1] = 2.0f * size[1] - 2.0f;
    ui_scrollbar->select_offset[0] = -size[0];
    ui_scrollbar->select_offset[1] = -size[1];
  }
}

static void ui_scrollbar_draw(const short area_id, const short ui_scrollbar_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(ui_scrollbar_id);

  update_dimensions(ui_scrollbar, area_id);
  UI_icon_draw_box(ui_scrollbar->select_scale[0], ui_scrollbar->select_scale[1], ui_scrollbar->select_offset[0], ui_scrollbar->select_offset[1]);

  if (UI_area_is_active(area_id)) {
    UI_icon_draw_box(ui_scrollbar->select_scale[0], ui_scrollbar->select_scale[1], ui_scrollbar->select_offset[0], ui_scrollbar->select_offset[1]);
  }
}

static void area_clear(const short id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(id);
  ui_scrollbar->area = 0;
}

static void update_area_size(UI_Scrollbar *ui_scrollbar, const short area_id)
{
  UI_area_set_size(area_id, 10, 20);
}

static void ui_scrollbar_area_attach(const short area_id, const short ui_scrollbar_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(ui_scrollbar_id);
  update_area_size(ui_scrollbar, area_id);
  area_clear(ui_scrollbar_id);
}

static void ui_scrollbar_area_resize(const short area_id, const short ui_scrollbar_id)
{
  area_clear(ui_scrollbar_id);
}

static void ui_scrollbar_start_drag(const short area_id, const short ui_scrollbar_id)
{
  UI_area_drag_start();
  UI_area_set_handled(area_id);
  UI_area_set_locked(area_id);
}

static void ui_scrollbar_end_drag(const short area_id, const short ui_scrollbar_id)
{
  UI_area_drag_end();
  UI_area_set_handled(area_id);
  UI_area_set_unlocked(area_id);
}

static short get_ui_scrollbar_key_map(void)
{
  if (!ui_scrollbar_key_map) {
    ui_scrollbar_key_map = Key_Map_create();
    Key_Map_add(ui_scrollbar_key_map, Key_Action_create(LEFT_BUTTON, ui_scrollbar_start_drag, 0));
    Key_Map_add(ui_scrollbar_key_map, Key_Action_create(LEFT_BUTTON_RELEASE, ui_scrollbar_end_drag, 0));
  }

  return ui_scrollbar_key_map;
}

static void ui_scrollbar_area_key_change(const short area_id, const short ui_scrollbar_id)
{
  Key_Map_action(get_ui_scrollbar_key_map(), UI_area_active_key(), area_id, ui_scrollbar_id);
}

static void ui_scrollbar_area_pointer_drag(const short area_id, const short ui_number_id)
{
  const short delta_y = UI_area_drag_delta_y();

  if (delta_y != 0) {
    UI_area_change_offset(area_id, 0, -delta_y);
  }
}

static short get_area_connector(void)
{
  if (!area_connector) {
    printf("create ui scrollbar area connector: ");
    area_connector = UI_area_connector(get_ui_scrollbar_class());
    UI_area_connect_draw(area_connector, ui_scrollbar_draw);
    UI_area_connect_resize(area_connector, ui_scrollbar_area_resize);
    UI_area_connect_attach(area_connector, ui_scrollbar_area_attach);
    UI_area_connect_key_change(area_connector, ui_scrollbar_area_key_change);
    UI_area_connect_pointer_drag(area_connector, ui_scrollbar_area_pointer_drag);
  }

  return area_connector;
}

static int get_ui_scrollbar_area_handle(const short ui_scrollbar_id)
{
  return Connector_handle(get_area_connector(), ui_scrollbar_id);
}

int UI_scrollbar_create(const short scroll_area)
{
  const short id = find_deleted_ui_scrollbar();
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(id);
  ui_scrollbar->active = 1;
  ui_scrollbar_init(ui_scrollbar);

  ui_scrollbar->scroll_area = scroll_area;

  return get_ui_scrollbar_area_handle(id);
}
