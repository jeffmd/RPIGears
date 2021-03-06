// ui_scrollbar.c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "id_plug.h"
#include "static_array.h"
#include "connector.h"
#include "key_map.h"
#include "key_action.h"
#include "key_input.h"
#include "tasks.h"
#include "ui_widget_connector.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "ui_icon.h"
#include "ui_slider.h"
#include "ui_button.h"

typedef struct {
  uint8_t active;
  ID_t area;
  Plug_t widget_plug;
  Plug_t inc_button;
  Plug_t dec_button;
  Plug_t slider;
  float normal_scale[2];
  float normal_offset[2];
  float select_scale[2];
  float select_offset[2];
} UI_Scrollbar;

typedef enum {
  SP_INC_BUTTON,
  SP_DEC_BUTTON,
  SP_SLIDER
} Part_ID;

typedef union {
  ID_t id;
  struct {
    ID_t scroll_id:13;
    ID_t part_id:3;
  };
} Scroll_Part_ID;

#define UI_SCROLLBAR_MAX_COUNT 50

static UI_Scrollbar ui_scrollbars[UI_SCROLLBAR_MAX_COUNT];
static ID_t next_deleted_ui_scrollbar;

static ID_t area_connector;
static ID_t parts_ui_connector;
static ID_t ui_scrollbar_class;
static ID_t ui_scrollbar_key_map;
static ID_t button_repeat_task;
static Scroll_Part_ID active_sp_id;

#define BUTTON_SIZE 20

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
  UI_area_set_size(area_id, 13, 150);
}

static void parts_ui_update(const ID_t source_id, const ID_t destination_id)
{
  const Scroll_Part_ID sp_id = {.id = destination_id};
  //UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(sp_id.scroll_id);

  switch (sp_id.part_id) {
    case SP_SLIDER:
      break;
  }
}

static void inc_button_pressed(const ID_t scrollbar_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(scrollbar_id);

  UI_slider_move_plus(ui_scrollbar->slider);
}

static void dec_button_pressed(const ID_t scrollbar_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(scrollbar_id);

  UI_slider_move_minus(ui_scrollbar->slider);
}

static void task_button_repeat(void)
{
  if (active_sp_id.id) {

    switch (active_sp_id.part_id) {
      case SP_INC_BUTTON:
        inc_button_pressed(active_sp_id.scroll_id);
        break;

      case SP_DEC_BUTTON:
        dec_button_pressed(active_sp_id.scroll_id);
        break;
    }
  }
}

static ID_t get_button_repeat_task(void)
{
  if (!button_repeat_task) {
    button_repeat_task = Task_create(100, task_button_repeat);
  }

  return button_repeat_task;
}

static void deactivate_button_repeat(void)
{
  active_sp_id.id = 0;

  Task_pause(get_button_repeat_task());
}

static void activate_button_repeat(const ID_t ui_scrollbar_id)
{
  active_sp_id.id = ui_scrollbar_id;

  task_button_repeat();
  Task_run(get_button_repeat_task());
}

static void parts_ui_changed(const ID_t source_id, const ID_t scrollbar_id)
{
  const Scroll_Part_ID sp_id = {.id = scrollbar_id};

  if (sp_id.part_id == SP_SLIDER) {

  }
  else { // its a button
    if(UI_button_pressed(source_id)) {
      activate_button_repeat(scrollbar_id);
    }
    else {
      deactivate_button_repeat();
    }
  }
}

static ID_t get_parts_ui_connector(void)
{
  if (!parts_ui_connector) {
    parts_ui_connector = UI_widget_connector(get_ui_scrollbar_class());
    UI_widget_connect_changed(parts_ui_connector, parts_ui_changed);
    UI_widget_connect_update(parts_ui_connector, parts_ui_update);
  }

  return parts_ui_connector;
}

static Plug_t get_part_ui_plug(ID_t scroll_id, Part_ID part_id)
{
  const Scroll_Part_ID sp_id = {.scroll_id = scroll_id, .part_id = part_id};

  return Connector_plug(get_parts_ui_connector(), sp_id.id);;
}

static Plug_t get_inc_button(ID_t scroll_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(scroll_id);

  if (!ui_scrollbar->inc_button) {
    ui_scrollbar->inc_button = UI_button_create("I", get_part_ui_plug(scroll_id, SP_INC_BUTTON));
  }

  return ui_scrollbar->inc_button;
}

static Plug_t get_dec_button(ID_t scroll_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(scroll_id);

  if (!ui_scrollbar->dec_button) {
    ui_scrollbar->dec_button = UI_button_create("D", get_part_ui_plug(scroll_id, SP_DEC_BUTTON));
  }

  return ui_scrollbar->dec_button;
}

static Plug_t get_slider(ID_t scroll_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(scroll_id);

  if (!ui_scrollbar->slider) {
    ui_scrollbar->slider = UI_slider_create(get_part_ui_plug(scroll_id, SP_SLIDER));
    UI_slider_set_full_travel(ui_scrollbar->slider, 110);
  }

  return ui_scrollbar->slider;
}

static void ui_scrollbar_area_attach(const ID_t area_id, const ID_t ui_scrollbar_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(ui_scrollbar_id);
  update_area_size(ui_scrollbar, area_id);
  area_clear(ui_scrollbar_id);

  UI_area_add_plug(area_id, get_slider(ui_scrollbar_id), 1, BUTTON_SIZE);
  UI_area_add_plug(area_id, get_dec_button(ui_scrollbar_id), 1, 0);
  UI_area_add_plug(area_id, get_inc_button(ui_scrollbar_id), 1, 130);
}

static void ui_scrollbar_area_resize(const ID_t area_id, const ID_t ui_scrollbar_id)
{
  area_clear(ui_scrollbar_id);
}

static void ui_scrollbar_click(const ID_t area_id, const ID_t ui_scrollbar_id)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(ui_scrollbar_id);
  const float travel_percent = (float)((UI_area_rel_pointer_y(area_id) - BUTTON_SIZE) * 2) / (ui_scrollbar->normal_scale[1] - (BUTTON_SIZE * 4) );

  UI_slider_set_travel_percent(ui_scrollbar->slider, travel_percent);
  UI_area_set_handled(area_id);
}

static void ui_scrollbar_inc(const ID_t area_id, const ID_t ui_scrollbar_id)
{
  inc_button_pressed(ui_scrollbar_id);
  UI_area_set_handled(area_id);
}

static void ui_scrollbar_dec(const ID_t area_id, const ID_t ui_scrollbar_id)
{
  dec_button_pressed(ui_scrollbar_id);
  UI_area_set_handled(area_id);
}

static ID_t get_ui_scrollbar_key_map(void)
{
  if (!ui_scrollbar_key_map) {
    ui_scrollbar_key_map = Key_Map_create();
    Key_Map_add(ui_scrollbar_key_map, Key_Action_create(LEFT_BUTTON, ui_scrollbar_click, 0));
    Key_Map_add(ui_scrollbar_key_map, Key_Action_create(WHEEL_DEC, ui_scrollbar_inc, 0));
    Key_Map_add(ui_scrollbar_key_map, Key_Action_create(WHEEL_INC, ui_scrollbar_dec, 0));
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

static Plug_t get_ui_scrollbar_area_plug(const ID_t ui_scrollbar_id)
{
  return Connector_plug(get_area_connector(), ui_scrollbar_id);
}

void UI_scrollbar_connect_widget(const ID_t ui_scrollbar_id, const Plug_t widget_plug)
{
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(ui_scrollbar_id);
  ui_scrollbar->widget_plug = widget_plug;
}

Plug_t UI_scrollbar_create(const Plug_t widget_plug)
{
  const ID_t id = find_deleted_ui_scrollbar();
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(id);
  ui_scrollbar->active = 1;
  ui_scrollbar_init(ui_scrollbar);
  UI_scrollbar_connect_widget(id, widget_plug);

  return get_ui_scrollbar_area_plug(id);
}
