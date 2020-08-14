// ui_checkbox.c

#include <stdio.h>
#include <stdint.h>

#include "key_map.h"
#include "key_action.h"

#include "key_input.h"
#include "connector.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "static_array.h"
#include "text.h"
#include "line_art.h"

typedef struct {
  uint8_t active;
  uint8_t selected;
  short text;
  short area;
  // Connector handle
  int handle;
  float box_offset[2];
} UI_CheckBox;

enum Events {
  OnSelect,
  OnUpdate,
  EventsMax
};

#define UI_CHECKBOX_MAX_COUNT 50
#define BOXSIZE 20
#define XOFFSET (BOXSIZE + 10)

static UI_CheckBox ui_checkboxes[UI_CHECKBOX_MAX_COUNT];
static short next_deleted_ui_checkbox;

static short area_connector;
static short ui_checkbox_class;
static short ui_checkbox_key_map;

static short checked_box_batch_part;
static short unchecked_box_batch_part;

static inline short find_deleted_ui_checkbox(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_ui_checkbox, ui_checkboxes, UI_CHECKBOX_MAX_COUNT, UI_CheckBox, "UI checkbox");
}

static UI_CheckBox *get_ui_checkbox(short id)
{
  if ((id < 0) | (id >= UI_CHECKBOX_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI checkbox id, using default id: 0\n");
  }
    
  return ui_checkboxes + id;
}

static void ui_checkbox_init(UI_CheckBox *ui_checkbox)
{
  ui_checkbox->area = 0;
}

/*
static void ui_checkbox_enter(const short source_id, const short destination_id)
{
  //printf("enter checkbox area %i\n", source_id);
}

static void ui_checkbox_leave(const short source_id, const short destination_id)
{
  //printf("leave checkbox area %i\n", source_id);
}
*/

static short get_text(UI_CheckBox *ui_checkbox)
{
  if (!ui_checkbox->text) {
    ui_checkbox->text = Text_create();
  }

  return ui_checkbox->text;
}

static void update_dimensions(UI_CheckBox *ui_checkbox, const short source_id)
{
  if (ui_checkbox->area != source_id)
  {
    int size[2];

    ui_checkbox->area = source_id;
    UI_area_size(source_id, size);
    Text_set_offset(get_text(ui_checkbox), size[0] - XOFFSET, size[1]);
    ui_checkbox->box_offset[0] = -size[0];
    ui_checkbox->box_offset[1] = -size[1] * 0.75f;
    //printf("update checkbox offset %i\n", text->text_id);
  }
}

static short get_unchecked_box_batch_part(void)
{
  if(!unchecked_box_batch_part) {
    unchecked_box_batch_part = Line_Art_create();
    Line_Art_add(0, 0);
    Line_Art_add(BOXSIZE, 0);
    Line_Art_add(BOXSIZE, BOXSIZE);
    Line_Art_add(0, BOXSIZE);
    Line_Art_add(0, 0);
    Line_Art_end(unchecked_box_batch_part);
  }

  return unchecked_box_batch_part;
}

static short get_checked_box_batch_part(void)
{
  if(!checked_box_batch_part) {
    checked_box_batch_part = Line_Art_create();
    Line_Art_add(BOXSIZE, BOXSIZE);
    Line_Art_add(0, BOXSIZE);
    Line_Art_add(0, 0);
    Line_Art_add(BOXSIZE, 0);
    Line_Art_add(BOXSIZE, BOXSIZE);
    Line_Art_add(BOXSIZE/2, BOXSIZE/3);
    Line_Art_add(BOXSIZE/4, BOXSIZE*2/3);
    Line_Art_end(checked_box_batch_part);
  }

  return checked_box_batch_part;
}

static void ui_checkbox_draw(const short source_id, const short destination_id)
{
  UI_CheckBox *const ui_checkbox = get_ui_checkbox(destination_id);
  Connector_handle_execute(ui_checkbox->handle, OnUpdate, destination_id);
  const short box_batch_part = ui_checkbox->selected ? get_checked_box_batch_part() : get_unchecked_box_batch_part();

  update_dimensions(ui_checkbox, source_id);
  Text_draw(get_text(ui_checkbox));
  Line_Art_set_offset(ui_checkbox->box_offset[0], ui_checkbox->box_offset[1]);
  Line_Art_draw(box_batch_part);
  //printf("draw checkbox area %i\n", source_id);
}

static void area_clear(const short id)
{
  UI_CheckBox *const ui_checkbox = get_ui_checkbox(id);
  ui_checkbox->area = 0;
}

static void update_area_size(UI_CheckBox *ui_checkbox, const short area_id)
{
  int extent[2];

  Text_extent(get_text(ui_checkbox), extent);
  UI_area_set_size(area_id, extent[0] + (XOFFSET / 2), extent[1]);
  //printf("checkbox area size x: %i, y: %i\n", extent[0], extent[1]); 
}

static void ui_checkbox_area_attach(const short source_id, const short destination_id)
{
  //printf("attach checkbox area %i\n", source_id);
  UI_CheckBox *const ui_checkbox = get_ui_checkbox(destination_id);
  update_area_size(ui_checkbox, source_id);
  area_clear(destination_id);
}

static void ui_checkbox_area_resize(const short source_id, const short destination_id)
{
  //printf("resize checkbox area %i\n", source_id);
  area_clear(destination_id);
}

static void ui_checkbox_select(const short source_id, const short destination_id)
{
  UI_CheckBox *const ui_checkbox = get_ui_checkbox(destination_id);
  Connector_handle_execute(ui_checkbox->handle, OnSelect, destination_id);
  UI_area_set_handled(source_id);
}

static short get_ui_checkbox_key_map(void)
{
  if (!ui_checkbox_key_map) {
    ui_checkbox_key_map = Key_Map_create();
    Key_Map_add(ui_checkbox_key_map, Key_Action_create(LEFT_BUTTON, ui_checkbox_select, 0));
  }

  return ui_checkbox_key_map;
}

static void ui_checkbox_area_key_change(const short source_id, const short destination_id)
{
  //printf("key change in checkbox area %i\n", source_id);
  Key_Map_action(get_ui_checkbox_key_map(), UI_area_active_key(), source_id, destination_id);
}

static short get_ui_checkbox_class(void)
{
  if(!ui_checkbox_class) {
    ui_checkbox_class = Connector_register_class("ui_checkbox");
  }

  return ui_checkbox_class;
}

static short get_area_connector(void)
{
  if (!area_connector) {
    //printf("create ui checkbox area connector: ");
    area_connector = UI_area_connector(get_ui_checkbox_class());
    //UI_area_connect_enter(area_connector, ui_checkbox_enter);
    //UI_area_connect_leave(area_connector, ui_checkbox_leave);
    UI_area_connect_draw(area_connector, ui_checkbox_draw);
    UI_area_connect_resize(area_connector, ui_checkbox_area_resize);
    UI_area_connect_attach(area_connector, ui_checkbox_area_attach);
    UI_area_connect_key_change(area_connector, ui_checkbox_area_key_change);
  }

  return area_connector;
}

static int get_ui_checkbox_area_handle(const short ui_checkbox_id)
{
  return Connector_handle(get_area_connector(), ui_checkbox_id);
}

int UI_checkbox_create(const char *str, const int handle)
{
  const short id = find_deleted_ui_checkbox();
  UI_CheckBox *const ui_checkbox = get_ui_checkbox(id);
  ui_checkbox->active = 1;
  ui_checkbox_init(ui_checkbox);
  Text_add(get_text(ui_checkbox), 0, 0, str);
  ui_checkbox->handle = handle;

  return get_ui_checkbox_area_handle(id);
}

short UI_checkbox_connector(const short destination_class)
{
  const short connector_id = Connector_create(get_ui_checkbox_class(), destination_class, EventsMax);

  return connector_id;
}

void UI_checkbox_connect(const short checkbox_id, const int handle)
{
  UI_CheckBox *const ui_checkbox = get_ui_checkbox(checkbox_id);
  ui_checkbox->handle = handle;
}

void UI_checkbox_connect_select(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnSelect, action);
}

void UI_checkbox_connect_update(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnUpdate, action);
}

void UI_checkbox_update_select(const short id, const int val)
{
  get_ui_checkbox(id)->selected = val;
}



