// ui_number.c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

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

typedef union 
{
  int i;
  float f;
} FPINT;

typedef struct {
  uint8_t active;
  uint8_t is_float:1;
  uint8_t can_edit:1;
  short text;
  short area;
  short val_start;
  short offset_x;
  FPINT old_val;
  FPINT change_val;
  FPINT default_change;
  int widget_handle;
  float select_scale[2];
  float select_offset[2];
} UI_Number;

#define UI_NUMBER_MAX_COUNT 50
#define DEFAULT_FLOAT_CHANGE 0.1f
#define DEFAULT_INT_CHANGE 1

static UI_Number ui_numbers[UI_NUMBER_MAX_COUNT];
static short next_deleted_ui_number;

static short area_connector;
static short ui_number_class;
static short ui_number_key_map;
static short old_y;
static short old_x;
static short drag_active;

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
    ui_number->select_scale[0] = 1.9f * size[0];
    ui_number->select_scale[1] = 1.9f * size[1];
    ui_number->select_offset[0] = -size[0];
    ui_number->select_offset[1] = -size[1] * 0.5f - 9.0f;
  }
}

static void update_value_text(UI_Number *const ui_number)
{
  Text_set_index(get_text(ui_number), ui_number->val_start);
  Text_add(get_text(ui_number), ui_number->offset_x, 0, val_str);
}

void UI_number_update_float(const short number_id, const float val)
{
  UI_Number *const ui_number = get_ui_number(number_id);

  if ( fabsf(ui_number->old_val.f - val) > 0.1f ) {
    ui_number->old_val.f = val;
    ui_number->is_float = 1;
    sprintf(val_str, "%3.1f  ", ui_number->old_val.f);
    update_value_text(ui_number);
  }
}

void UI_number_update_int(const short number_id, const int val)
{
  UI_Number *const ui_number = get_ui_number(number_id);

  if ( abs(ui_number->old_val.i - val) > 0 ) {
    ui_number->old_val.i = val;
    ui_number->is_float = 0;
    sprintf(val_str, "%3i    ", ui_number->old_val.i);
    update_value_text(ui_number);
  }
}

static void ui_number_draw(const short source_id, const short destination_id)
{
  UI_Number *const ui_number = get_ui_number(destination_id);
  UI_widget_update(ui_number->widget_handle, destination_id);

  update_dimensions(ui_number, source_id);
  Text_draw(get_text(ui_number));

  if (ui_number->can_edit && UI_area_is_active(source_id)) {
    UI_icon_draw_box(ui_number->select_scale[0], ui_number->select_scale[1], ui_number->select_offset[0], ui_number->select_offset[1]);
  }
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

static float get_default_float_change(UI_Number *const ui_number)
{
  if (ui_number->default_change.f == 0.0f) {
    ui_number->default_change.f = DEFAULT_FLOAT_CHANGE;
  }
  
  return ui_number->default_change.f;
}

static int get_default_int_change(UI_Number *const ui_number)
{
  if (ui_number->default_change.i == 0) {
    ui_number->default_change.i = DEFAULT_INT_CHANGE;
  }
  
  return ui_number->default_change.i;
}

static void ui_number_inc(const short source_id, const short destination_id)
{
  UI_Number *const ui_number = get_ui_number(destination_id);

  if (ui_number->is_float) {
    ui_number->change_val.f = get_default_float_change(ui_number);
  }
  else {
    ui_number->change_val.i = get_default_int_change(ui_number);
  }

  UI_widget_change(ui_number->widget_handle, destination_id);
}

static void ui_number_dec(const short source_id, const short destination_id)
{
  UI_Number *const ui_number = get_ui_number(destination_id);

  if (ui_number->is_float) {
    ui_number->change_val.f = -get_default_float_change(ui_number);
  }
  else {
    ui_number->change_val.i = -get_default_int_change(ui_number);
  }

  UI_widget_change(ui_number->widget_handle, destination_id);
}


static void ui_number_start_change(const short source_id, const short destination_id)
{
  //UI_Number *const ui_number = get_ui_number(destination_id);
  //ui_number_inc(ui_number);
  drag_active = 1;
  old_y = UI_area_pointer_y();
  old_x = UI_area_pointer_x();
  UI_area_set_handled(source_id);
  UI_area_set_locked(source_id);
}

static void ui_number_end_change(const short source_id, const short destination_id)
{
  drag_active = 0;
  UI_area_set_handled(source_id);
  UI_area_set_unlocked(source_id);
}

static short get_ui_number_key_map(void)
{
  if (!ui_number_key_map) {
    ui_number_key_map = Key_Map_create();
    Key_Map_add(ui_number_key_map, Key_Action_create(MIDDLE_BUTTON, ui_number_start_change, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(MIDDLE_BUTTON_RELEASE, ui_number_end_change, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(WHEEL_INC, ui_number_inc, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(WHEEL_DEC, ui_number_dec, 0));
  }

  return ui_number_key_map;
}

static void ui_number_area_key_change(const short source_id, const short destination_id)
{
  if (get_ui_number(destination_id)->can_edit) {
    Key_Map_action(get_ui_number_key_map(), UI_area_active_key(), source_id, destination_id);
  }
}

static void ui_number_area_pointer_move(const short source_id, const short destination_id)
{
  if (drag_active) {
    short new_y = UI_area_pointer_y();
    short new_x = UI_area_pointer_x();
    short delta = (old_y - new_y) + (new_x - old_x);

    if (delta > 0) {
      ui_number_inc(source_id, destination_id);
    }
    else if (delta < 0) {
      ui_number_dec(source_id, destination_id);
    }

    old_y = new_y;
    old_x = new_x;
  }
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
    UI_area_connect_key_change(area_connector, ui_number_area_key_change);
    UI_area_connect_pointer_move(area_connector, ui_number_area_pointer_move);
  }

  return area_connector;
}

static int get_ui_number_area_handle(const short ui_number_id)
{
  return Connector_handle(get_area_connector(), ui_number_id);
}

void UI_number_connect_widget(const short number_id, const int handle)
{
  UI_Number *const ui_number = get_ui_number(number_id);
  ui_number->widget_handle = handle;
}

float UI_number_float_change(const short number_id)
{
  return get_ui_number(number_id)->change_val.f;
}

int UI_number_int_change(const short number_id)
{
  return get_ui_number(number_id)->change_val.i;
}

void UI_number_set_default_float_change(const short number_id, const float val)
{
  get_ui_number(number_id)->default_change.f = val;
}

void UI_number_set_default_int_change(const short number_id, const int val)
{
  get_ui_number(number_id)->default_change.i = val;
}

void UI_number_edit_on(const short number_id)
{
  get_ui_number(number_id)->can_edit = 1;
}

void UI_number_edit_off(const short number_id)
{
  get_ui_number(number_id)->can_edit = 0;
}

int UI_number_create(const char *str, const int handle)
{
  const short id = find_deleted_ui_number();
  UI_Number *const ui_number = get_ui_number(id);
  ui_number->active = 1;
  ui_number_init(ui_number);
  Text_add(get_text(ui_number), 0, 0, str);
  setup_val_text(ui_number);
  UI_number_connect_widget(id, handle);

  return get_ui_number_area_handle(id);
}


