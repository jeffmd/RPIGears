// ui_scrollbar.c

#include <stdio.h>
#include <stdint.h>

#include "static_array.h"
#include "connector.h"
#include "key_map.h"
#include "key_action.h"
#include "key_input.h"
#include "ui_widget_connector.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "ui_icon.h"
#include "ui_slider.h"
#include "ui_button.h"

typedef struct {
  uint8_t active;
  ID_t area;
  Handle_t widget_handle;
  Handle_t inc_button;
  Handle_t dec_button;
  Handle_t slider;
  float normal_scale[2];
  float normal_offset[2];
  float select_scale[2];
  float select_offset[2];
} UI_Scrollbar;

#define UI_SCROLLBAR_MAX_COUNT 50

static UI_Scrollbar ui_scrollbars[UI_SCROLLBAR_MAX_COUNT];
static ID_t next_deleted_ui_scrollbar;

static ID_t area_connector;
static ID_t ui_scrollbar_class;
static ID_t ui_scrollbar_key_map;

static inline ID_t find_deleted_ui_scrollbar(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_ui_scrollbar, ui_scrollbars, UI_SCROLLBAR_MAX_COUNT, UI_Scrollbar, "UI scrollbar");
}

static UI_Scrollbar *get_ui_scrollbar(ID_t id)
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

static ID_t get_ui_scrollbar_class(void)
{
  if(!ui_scrollbar_class) {
    ui_scrollbar_class = Connector_register_class("ui_scrollbar");
  }

  return ui_scrollbar_class;
}

static void update_dimensions(UI_Scrollbar *ui_scrollbar, const ID_t area_id)
{
  if (ui_scrollbar->area != area_id)
  {
    short size[2];

    ui_scrollbar->area = area_id;
    UI_area_size(area_id, size);

    ui_scrollbar->normal_scale[0] = 2.0f * size[0] - 2.0f;
    ui_scrollbar->normal_scale[1] = 2.0f * size[1] - 2.0f;
    ui_scrollbar->normal_offset[0] = -size[0];
    ui_scrollbar->normal_offset[1] = -size[1];

    ui_scrollbar->select_scale[0] = 2.0f * size[0] - 6.0f;
    ui_scrollbar->select_scale[1] = 2.0f * size[1] - 6.0f;
    ui_scrollbar->select_offset[0] = -size[0] + 2.0f;
    ui_scrollbar->select_offset[1] = -size[1] + 2.0f;
  }
}

static void ui_scrollbar_draw(const ID_t area_id, const ID_t ui_scrollbar_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(ui_scrollbar_id);

  update_dimensions(ui_scrollbar, area_id);
  UI_icon_draw_box(ui_scrollbar->normal_scale[0], ui_scrollbar->normal_scale[1], ui_scrollbar->normal_offset[0], ui_scrollbar->normal_offset[1]);

  if (UI_area_is_active(area_id)) {
    UI_icon_draw_box(ui_scrollbar->select_scale[0], ui_scrollbar->select_scale[1], ui_scrollbar->select_offset[0], ui_scrollbar->select_offset[1]);

  }
}

static void area_clear(const ID_t id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(id);
  ui_scrollbar->area = 0;
}

static void update_area_size(UI_Scrollbar *ui_scrollbar, const ID_t area_id)
{
  UI_area_set_size(area_id, 10, 20);
}

static void ui_scrollbar_area_attach(const ID_t area_id, const ID_t ui_scrollbar_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(ui_scrollbar_id);
  update_area_size(ui_scrollbar, area_id);
  area_clear(ui_scrollbar_id);
}

static void ui_scrollbar_area_resize(const ID_t area_id, const ID_t ui_scrollbar_id)
{
  area_clear(ui_scrollbar_id);
}

static void ui_scrollbar_start_drag(const ID_t area_id, const ID_t ui_scrollbar_id)
{
}

static ID_t get_ui_scrollbar_key_map(void)
{
  if (!ui_scrollbar_key_map) {
    ui_scrollbar_key_map = Key_Map_create();
    Key_Map_add(ui_scrollbar_key_map, Key_Action_create(LEFT_BUTTON, ui_scrollbar_start_drag, 0));
  }

  return ui_scrollbar_key_map;
}

static void ui_scrollbar_area_key_change(const ID_t area_id, const ID_t ui_scrollbar_id)
{
  Key_Map_action(get_ui_scrollbar_key_map(), UI_area_active_key(), area_id, ui_scrollbar_id);
}

static ID_t get_area_connector(void)
{
  if (!area_connector) {
    printf("create ui scrollbar area connector: ");
    area_connector = UI_area_connector(get_ui_scrollbar_class());
    UI_area_connect_draw(area_connector, ui_scrollbar_draw);
    UI_area_connect_resize(area_connector, ui_scrollbar_area_resize);
    UI_area_connect_attach(area_connector, ui_scrollbar_area_attach);
    UI_area_connect_key_change(area_connector, ui_scrollbar_area_key_change);
  }

  return area_connector;
}

static Handle_t get_ui_scrollbar_area_handle(const ID_t ui_scrollbar_id)
{
  return Connector_handle(get_area_connector(), ui_scrollbar_id);
}

void UI_scrollbar_connect_widget(const ID_t ui_scrollbar_id, const Handle_t widget_handle)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(ui_scrollbar_id);
  ui_scrollbar->widget_handle = widget_handle;
}

Handle_t UI_scrollbar_create(const Handle_t widget_handle)
{
  const ID_t id = find_deleted_ui_scrollbar();
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(id);
  ui_scrollbar->active = 1;
  ui_scrollbar_init(ui_scrollbar);
  UI_scrollbar_connect_widget(id, widget_handle);

  return get_ui_scrollbar_area_handle(id);
}
