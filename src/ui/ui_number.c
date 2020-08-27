// ui_number.c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "connector.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "static_array.h"
#include "text.h"

typedef union 
{
  int i;
  float f;
} FPINT;

typedef struct {
  uint8_t active;
  short text;
  short area;
  short val_start;
  short offset_x;
  FPINT old_val;
  int handle;
} UI_Number;

enum Events {
  OnChange,
  OnUpdate,
  EventsMax
};

#define UI_NUMBER_MAX_COUNT 50

static UI_Number ui_numbers[UI_NUMBER_MAX_COUNT];
static short next_deleted_ui_number;

static short area_connector;
static short ui_number_class;

static const char num_str[] = "000.000";
static char val_str[12];

static inline short find_deleted_ui_number(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_ui_number, ui_numbers, UI_NUMBER_MAX_COUNT, UI_Number, "UI number");
}

static UI_Number *get_ui_number(short id)
{
  if ((id < 0) | (id >= UI_NUMBER_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI number id, using default id: 0\n");
  }
    
  return ui_numbers + id;
}

static void ui_number_init(UI_Number *ui_number)
{
  ui_number->area = 0;
}

static short get_text(UI_Number *ui_number)
{
  if (!ui_number->text) {
    ui_number->text = Text_create();
  }

  return ui_number->text;
}

static void setup_val_text(UI_Number *ui_number)
{
  const short text_id = get_text(ui_number);
  ui_number->val_start = Text_index(text_id);
  ui_number->offset_x = Text_pos_x(text_id) + 10;
  Text_add(text_id, ui_number->offset_x, 0, num_str);
}

static void update_dimensions(UI_Number *ui_number, const short source_id)
{
  if (ui_number->area != source_id)
  {
    int size[2];

    ui_number->area = source_id;
    UI_area_size(source_id, size);
    Text_set_offset(get_text(ui_number), size[0], size[1]);
    //printf("update text offset %i\n", text->text_id);
  }
}

static void update_value_text(UI_Number *const ui_number, const int is_float)
{
  if (is_float) {
    sprintf(val_str, "%3.1f  ", ui_number->old_val.f);
  }
  else {
    sprintf(val_str, "%3i    ", ui_number->old_val.i);
  }

  Text_set_index(get_text(ui_number), ui_number->val_start);
  Text_add(get_text(ui_number), ui_number->offset_x, 0, val_str);
}

void UI_number_update_float(const short number_id, const float val)
{
  UI_Number *const ui_number = get_ui_number(number_id);

  if ( fabsf(ui_number->old_val.f - val) > 0.1f ) {
    ui_number->old_val.f = val;
    update_value_text(ui_number, 1);
  }
}

void UI_number_update_int(const short number_id, const int val)
{
  UI_Number *const ui_number = get_ui_number(number_id);

  if ( abs(ui_number->old_val.i - val) > 0 ) {
    ui_number->old_val.i = val;
    update_value_text(ui_number, 0);
  }
}

static void ui_number_draw(const short source_id, const short destination_id)
{
  UI_Number *const ui_number = get_ui_number(destination_id);
  Connector_handle_execute(ui_number->handle, OnUpdate, destination_id);

  update_dimensions(ui_number, source_id);
  Text_draw(get_text(ui_number));
  //printf("draw text area %i\n", source_id);
}

static void area_clear(const short id)
{
  UI_Number *const ui_number = get_ui_number(id);
  ui_number->area = 0;
}

static void update_area_size(UI_Number *ui_number, const short area_id)
{
  int extent[2];

  Text_extent(get_text(ui_number), extent);
  UI_area_set_size(area_id, extent[0], extent[1]);
  //printf("text area size x: %i, y: %i\n", extent[0], extent[1]); 
}

static void ui_number_area_attach(const short source_id, const short destination_id)
{
  //printf("attach text area %i\n", source_id);
  UI_Number *const ui_number = get_ui_number(destination_id);
  update_area_size(ui_number, source_id);
  area_clear(destination_id);
}

static void ui_number_area_resize(const short source_id, const short destination_id)
{
  //printf("resize text area %i\n", source_id);
  area_clear(destination_id);
}

static short get_ui_number_class(void)
{
  if(!ui_number_class) {
    ui_number_class = Connector_register_class("ui_number");
  }

  return ui_number_class;
}

static short get_area_connector(void)
{
  if (!area_connector) {
    printf("create ui number area connector: ");
    area_connector = UI_area_connector(get_ui_number_class());
    //UI_area_connect_enter(area_connector, ui_number_enter);
    //UI_area_connect_leave(area_connector, ui_number_leave);
    UI_area_connect_draw(area_connector, ui_number_draw);
    UI_area_connect_resize(area_connector, ui_number_area_resize);
    UI_area_connect_attach(area_connector, ui_number_area_attach);
  }

  return area_connector;
}

static int get_ui_number_area_handle(const short ui_number_id)
{
  return Connector_handle(get_area_connector(), ui_number_id);
}

int UI_number_create(const char *str, const int handle)
{
  const short id = find_deleted_ui_number();
  UI_Number *const ui_number = get_ui_number(id);
  ui_number->active = 1;
  ui_number_init(ui_number);
  Text_add(get_text(ui_number), 0, 0, str);
  setup_val_text(ui_number);
  ui_number->handle = handle;

  return get_ui_number_area_handle(id);
}

short UI_number_connector(const short destination_class)
{
  const short connector_id = Connector_create(get_ui_number_class(), destination_class, EventsMax);

  return connector_id;
}

void UI_number_connect(const short number_id, const int handle)
{
  UI_Number *const ui_number = get_ui_number(number_id);
  ui_number->handle = handle;
}

void UI_number_connect_change(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnChange, action);
}

void UI_number_connect_update(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnUpdate, action);
}


