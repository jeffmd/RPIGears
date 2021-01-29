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
#include "text.h"
#include "ui_icon.h"

typedef struct {
  uint8_t active;
  uint8_t pressed:1;
  short area;
  short text;
  int widget_handle;
  float normal_scale[2];
  float normal_offset[2];
  float select_scale[2];
  float select_offset[2];
  float select2_scale[2];
  float select2_offset[2];
} UI_Button;

#define UI_BUTTON_MAX_COUNT 50
#define BOXSIZE 20
#define XOFFSET (BOXSIZE + 10)

static UI_Button ui_buttons[UI_BUTTON_MAX_COUNT];
static short next_deleted_ui_button;

static short area_connector;
static short ui_button_class;
static short ui_button_key_map;

static inline short find_deleted_ui_button(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_ui_button, ui_buttons, UI_BUTTON_MAX_COUNT, UI_Button, "UI button");
}

static UI_Button *get_ui_button(short id)
{
  if ((id < 0) | (id >= UI_BUTTON_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI button id, using default id: 0\n");
  }
    
  return ui_buttons + id;
}

static void ui_button_init(UI_Button *ui_button)
{

}

static short get_ui_button_class(void)
{
  if(!ui_button_class) {
    ui_button_class = Connector_register_class("ui_button");
  }

  return ui_button_class;
}

static short get_text(UI_Button *ui_button)
{
  if (!ui_button->text) {
    ui_button->text = Text_create();
  }

  return ui_button->text;
}

static void update_dimensions(UI_Button *ui_button, const short area_id)
{
  if (ui_button->area != area_id)
  {
    short size[2];

    ui_button->area = area_id;
    UI_area_size(area_id, size);
    Text_set_offset(get_text(ui_button), size[0] - XOFFSET, size[1]);

    ui_button->normal_scale[0] = 2.0f * size[0] - 2.0f;
    ui_button->normal_scale[1] = 2.0f * size[1] - 2.0f;
    ui_button->normal_offset[0] = -size[0];
    ui_button->normal_offset[1] = -size[1];

    ui_button->select_scale[0] = 2.0f * size[0] - 6.0f;
    ui_button->select_scale[1] = 2.0f * size[1] - 6.0f;
    ui_button->select_offset[0] = -size[0] + 2.0f;
    ui_button->select_offset[1] = -size[1] + 2.0f;

    ui_button->select2_scale[0] = 2.0f * size[0] - 10.0f;
    ui_button->select2_scale[1] = 2.0f * size[1] - 10.0f;
    ui_button->select2_offset[0] = -size[0] + 4.0f;
    ui_button->select2_offset[1] = -size[1] + 4.0f;
  }
}

static void ui_button_draw(const short area_id, const short ui_button_id)
{
  UI_Button *const ui_button = get_ui_button(ui_button_id);

  update_dimensions(ui_button, area_id);
  Text_draw(get_text(ui_button));
  UI_icon_draw_box(ui_button->normal_scale[0], ui_button->normal_scale[1], ui_button->normal_offset[0], ui_button->normal_offset[1]);

  if (UI_area_is_active(area_id)) {
    UI_icon_draw_box(ui_button->select_scale[0], ui_button->select_scale[1], ui_button->select_offset[0], ui_button->select_offset[1]);

    if (ui_button->pressed) {
      UI_icon_draw_box(ui_button->select2_scale[0], ui_button->select2_scale[1], ui_button->select2_offset[0], ui_button->select2_offset[1]);
    }
  }
}

static void area_clear(const short id)
{
  UI_Button *const ui_button = get_ui_button(id);
  ui_button->area = 0;
}

static void update_area_size(UI_Button *ui_button, const short area_id)
{
  int extent[2];

  Text_extent(get_text(ui_button), extent);
  UI_area_set_size(area_id, extent[0] + (XOFFSET / 2), extent[1]);
}

static void ui_button_area_attach(const short area_id, const short ui_button_id)
{
  UI_Button *const ui_button = get_ui_button(ui_button_id);
  update_area_size(ui_button, area_id);
  area_clear(ui_button_id);
}

static void ui_button_area_resize(const short area_id, const short ui_button_id)
{
  area_clear(ui_button_id);
}

static void ui_button_press(const short area_id, const short ui_button_id)
{
  UI_Button *const ui_button = get_ui_button(ui_button_id);
  ui_button->pressed = 1;
  UI_widget_changed(ui_button->widget_handle, ui_button_id);
  UI_area_set_handled(area_id);
  UI_area_set_locked(area_id);
}

static void ui_button_release(const short area_id, const short ui_button_id)
{
  UI_Button *const ui_button = get_ui_button(ui_button_id);
  ui_button->pressed = 0;
  UI_widget_changed(ui_button->widget_handle, ui_button_id);
  UI_area_set_handled(area_id);
  UI_area_set_unlocked(area_id);
}

static short get_ui_button_key_map(void)
{
  if (!ui_button_key_map) {
    ui_button_key_map = Key_Map_create();
    Key_Map_add(ui_button_key_map, Key_Action_create(LEFT_BUTTON, ui_button_press, 0));
    Key_Map_add(ui_button_key_map, Key_Action_create(LEFT_BUTTON_RELEASE, ui_button_release, 0));
  }

  return ui_button_key_map;
}

static void ui_button_area_key_change(const short area_id, const short ui_button_id)
{
  Key_Map_action(get_ui_button_key_map(), UI_area_active_key(), area_id, ui_button_id);
}

static short get_area_connector(void)
{
  if (!area_connector) {
    printf("create ui button area connector: ");
    area_connector = UI_area_connector(get_ui_button_class());
    UI_area_connect_draw(area_connector, ui_button_draw);
    UI_area_connect_resize(area_connector, ui_button_area_resize);
    UI_area_connect_attach(area_connector, ui_button_area_attach);
    UI_area_connect_key_change(area_connector, ui_button_area_key_change);
  }

  return area_connector;
}

static int get_ui_button_area_handle(const short ui_button_id)
{
  return Connector_handle(get_area_connector(), ui_button_id);
}

void UI_button_connect_widget(const short ui_button_id, const int handle)
{
  UI_Button *const ui_button = get_ui_button(ui_button_id);
  ui_button->widget_handle = handle;
}

int UI_button_create(const char *str, const int handle)
{
  const short id = find_deleted_ui_button();
  UI_Button *const ui_button = get_ui_button(id);
  ui_button->active = 1;
  ui_button_init(ui_button);
  Text_add(get_text(ui_button), 0, 0, str);
  UI_button_connect_widget(id, handle);

  return get_ui_button_area_handle(id);
}
