// ui_number.c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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
#include "tasks.h"

typedef union 
{
  int i;
  float f;
} FPINT;

typedef struct
{
  uint8_t changed:1;
  uint8_t cursor_moved:1;
  uint8_t cursor_blink:1;
} EditFlags;

typedef struct {
  uint8_t active;
  uint8_t is_float:1;
  uint8_t can_edit:1;
  uint8_t editing:1;
  short text;
  short area;
  short val_start_index;
  short val_offset_x;
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
static short delta_xy;

#define STR_SIZE 12
static const char num_str[] = "000.000 ";
static char val_str[STR_SIZE];

static char edit_str[STR_SIZE];
static EditFlags edit_flags;
static short edit_ui_number;
static short edit_cursor_index;
static short cursor_blink_task;
static float edit_offset_x;
static FPINT edit_restore_val;

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
  ui_number->val_start_index = Text_index(text_id);
  ui_number->val_offset_x = Text_pos_x(text_id) + 12;
  Text_add(text_id, ui_number->val_offset_x, 0, num_str);
}

static void update_dimensions(UI_Number *ui_number, const short source_id)
{
  if (ui_number->area != source_id)
  {
    short size[2];

    ui_number->area = source_id;
    UI_area_size(source_id, size);
    Text_set_offset(get_text(ui_number), size[0] - 2, size[1]);
    ui_number->select_scale[0] = 2.0f * size[0] - 2.0f;
    ui_number->select_scale[1] = 2.0f * size[1] - 2.0f;
    ui_number->select_offset[0] = -size[0];
    ui_number->select_offset[1] = -size[1];
  }
}

static void update_value_text(UI_Number *const ui_number, const char *str)
{
  Text_set_index(get_text(ui_number), ui_number->val_start_index);
  Text_add(get_text(ui_number), ui_number->val_offset_x, 0, str);
}

static void make_float_str(char *str, const float val)
{
  sprintf(str, "%-6.1f ", val);
}

static void update_float(UI_Number *const ui_number, const float val)
{
  ui_number->old_val.f = val;
  ui_number->is_float = 1;
  make_float_str(val_str, val);
  update_value_text(ui_number, val_str);
}

void UI_number_update_float(const short number_id, const float val)
{
  UI_Number *const ui_number = get_ui_number(number_id);

  if ( fabsf(ui_number->old_val.f - val) > 0.1f ) {
    update_float(ui_number, val);
  }
}

static void make_int_str(char *str, const int val)
{
  sprintf(str, "%-6i ", val);
}

static void update_int(UI_Number *const ui_number, const int val)
{
  ui_number->old_val.i = val;
  ui_number->is_float = 0;
  make_int_str(val_str, val);
  update_value_text(ui_number, val_str);
}

void UI_number_update_int(const short number_id, const int val)
{
  UI_Number *const ui_number = get_ui_number(number_id);

  if ( abs(ui_number->old_val.i - val) > 0 ) {
    update_int(ui_number, val);
  }
}

static void task_blink_cursor(void)
{
  edit_flags.cursor_blink = !edit_flags.cursor_blink;
}

static short get_cursor_blink_task(void)
{
  if (!cursor_blink_task) {
    cursor_blink_task = Task_create(500, task_blink_cursor);
  }

  return cursor_blink_task;
}

static void edit_cursor_blink_task_start(void)
{
  Task_run(get_cursor_blink_task());
}

static void edit_cursor_blink_task_stop(void)
{
  Task_pause(get_cursor_blink_task());
}

static void edit_changed(void)
{
  edit_flags.changed = 1;
  edit_flags.cursor_moved = 1;
  edit_flags.cursor_blink = 1;
  edit_cursor_blink_task_start();
}

static void edit_val_update(UI_Number *const ui_number, FPINT val)
{
  if (ui_number->is_float) {
    update_float(ui_number, val.f);
  }
  else {
    update_int(ui_number, val.i);
  }
}

static void edit_str_update(UI_Number *const ui_number, FPINT val)
{
  if (ui_number->is_float) {
    make_float_str(edit_str, val.f);
  }
  else {
    make_int_str(edit_str, val.i);
  }
}

static void edit_cursor_update(UI_Number *const ui_number)
{
  if (edit_flags.cursor_moved) {
    const short text = get_text(ui_number);
    Text_set_index(text, edit_cursor_index + ui_number->val_start_index);
    Text_sync_pos(text);
    edit_offset_x = Text_pos_x(text) + ui_number->select_offset[0];
    edit_flags.cursor_moved = 0;
  }
}

static void edit_text_update(UI_Number *const ui_number)
{
  if (edit_flags.changed) {
    update_value_text(ui_number, edit_str);
    edit_flags.changed = 0;
  }
}

static void edit_start(const short ui_number_id)
{
  UI_Number *const ui_number = get_ui_number(ui_number_id);
  ui_number->editing = 1;
  edit_restore_val = ui_number->old_val;
  edit_str_update(ui_number, ui_number->old_val);
  edit_ui_number = ui_number_id;
}

static void edit_stop(const short ui_number_id)
{
  UI_Number *const ui_number = get_ui_number(ui_number_id);
  ui_number->editing = 0;
  ui_number->old_val = edit_restore_val;
  edit_val_update(ui_number, edit_restore_val);
  edit_ui_number = 0;
  edit_cursor_blink_task_stop();
}

static void edit_draw_cursor(UI_Number *ui_number)
{
  // draw cursor at edit index position

  if (ui_number->editing && edit_flags.cursor_blink) {
    edit_cursor_update(ui_number);
    UI_icon_draw_box(1.0f, ui_number->select_scale[1], edit_offset_x, ui_number->select_offset[1]);
  }
}

static void ui_number_draw(const short source_id, const short destination_id)
{
  UI_Number *const ui_number = get_ui_number(destination_id);

  if (ui_number->editing) {
    edit_text_update(ui_number);
  }
  else {
    UI_widget_update(ui_number->widget_handle, destination_id);
  }

  update_dimensions(ui_number, source_id);

  Text_draw(get_text(ui_number));

  if (ui_number->can_edit && UI_area_is_active(source_id)) {
    edit_draw_cursor(ui_number);
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

static void ui_number_edit_done(const short source_id, const short destination_id)
{
  UI_Number *const ui_number = get_ui_number(destination_id);

  if (ui_number->editing) {
    edit_stop(destination_id);
    UI_area_set_handled(source_id);
    UI_area_set_unlocked(source_id);
  }

  edit_ui_number = 0;
}

static float get_delta_xy(void)
{
  int key_state = UI_area_active_key();
  float scale = 1.0f;

  if (key_state & SHIFT_KEY(0)) {
    scale = 10.0f;  
  }
  else if (key_state & CTRL_KEY(0)) {
    scale = 0.1f;  
  }

  return delta_xy * scale;
}

static float get_default_float_change(UI_Number *const ui_number)
{
  if (ui_number->default_change.f == 0.0f) {
    ui_number->default_change.f = DEFAULT_FLOAT_CHANGE;
  }
  
  return ui_number->default_change.f * get_delta_xy();
}

static int get_default_int_change(UI_Number *const ui_number)
{
  if (ui_number->default_change.i == 0) {
    ui_number->default_change.i = DEFAULT_INT_CHANGE;
  }
  
  return ui_number->default_change.i * get_delta_xy();
}

static void ui_number_inc(const short source_id, const short destination_id)
{
  ui_number_edit_done(source_id, destination_id);
  UI_Number *const ui_number = get_ui_number(destination_id);

  if (ui_number->is_float) {
    ui_number->change_val.f = get_default_float_change(ui_number);
  }
  else {
    ui_number->change_val.i = get_default_int_change(ui_number);
  }

  UI_widget_changed(ui_number->widget_handle, destination_id);
  UI_area_set_handled(source_id);
}

static void ui_number_dec(const short source_id, const short destination_id)
{
  delta_xy = -1;
  ui_number_inc(source_id, destination_id);
}

static void ui_number_edit(const short source_id, const short destination_id)
{
  if (edit_ui_number != destination_id) {
    edit_start(destination_id);
    UI_area_set_locked(source_id);
  }

  if (UI_area_locked_hit()) {
    edit_changed();
  }
  else {
    ui_number_edit_done(source_id, destination_id);
  }

  UI_area_set_handled(source_id);
}

static void ui_number_undo_edit(const short source_id, const short destination_id)
{
  ui_number_edit_done(source_id, destination_id);
}

static void ui_number_start_drag(const short source_id, const short destination_id)
{
  ui_number_edit_done(source_id, destination_id);
  UI_area_drag_start();
  UI_area_set_handled(source_id);
  UI_area_set_locked(source_id);
}

static void ui_number_end_drag(const short source_id, const short destination_id)
{
  UI_area_drag_end();
  UI_area_set_handled(source_id);
  UI_area_set_unlocked(source_id);
}

static short get_ui_number_key_map(void)
{
  if (!ui_number_key_map) {
    ui_number_key_map = Key_Map_create();
    Key_Map_add(ui_number_key_map, Key_Action_create(LEFT_BUTTON, ui_number_edit, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(ESC_KEY, ui_number_undo_edit, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(MIDDLE_BUTTON, ui_number_start_drag, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(SHIFT_KEY(MIDDLE_BUTTON), ui_number_start_drag, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(CTRL_KEY(MIDDLE_BUTTON), ui_number_start_drag, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(MIDDLE_BUTTON_RELEASE, ui_number_end_drag, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(WHEEL_INC, ui_number_inc, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(WHEEL_DEC, ui_number_dec, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(SHIFT_KEY(WHEEL_INC), ui_number_inc, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(SHIFT_KEY(WHEEL_DEC), ui_number_dec, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(CTRL_KEY(WHEEL_INC), ui_number_inc, 0));
    Key_Map_add(ui_number_key_map, Key_Action_create(CTRL_KEY(WHEEL_DEC), ui_number_dec, 0));
  }

  return ui_number_key_map;
}

static void ui_number_area_key_change(const short source_id, const short destination_id)
{
  if (get_ui_number(destination_id)->can_edit) {
    delta_xy = 1;
    // if editing then check edit keys first
    if (get_ui_number(destination_id)->editing) {
      printf("editing key: %i\n", UI_area_active_key());
    }
    // if not handled then do number key map
    Key_Map_action(get_ui_number_key_map(), UI_area_active_key(), source_id, destination_id);
  }
}

static void ui_number_area_pointer_drag(const short source_id, const short destination_id)
{
  delta_xy = UI_area_drag_delta_xy();
  ui_number_inc(source_id, destination_id);
  delta_xy = 1;
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
    UI_area_connect_pointer_drag(area_connector, ui_number_area_pointer_drag);
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

void UI_number_set_edit(const short number_id, const int state)
{
  get_ui_number(number_id)->can_edit = state ? 1 : 0;
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


