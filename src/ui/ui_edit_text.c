// ui_edit_text.c

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "key_map.h"
#include "key_action.h"
#include "key_input.h"

#include "ui_area_action.h"
#include "tasks.h"

typedef struct
{
  uint8_t changed:1;
  uint8_t cursor_moved:1;
  uint8_t cursor_blink:1;
  uint8_t insert_mode:1;
} EditFlags;

static char *edit_str;
static short edit_str_length;
static EditFlags edit_flags;
static short edit_key_map;
static short edit_cursor_index;
static short cursor_blink_task;

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

static void edit_cursor_changed(void)
{
  edit_flags.cursor_moved = 1;
  edit_flags.cursor_blink = 1;
  edit_cursor_blink_task_start();
}

int UI_edit_text_set_cursor_index(short index)
{
  int changed = 0;

  if (index < 0) {
    index = 0;
  }
  else if (index >= edit_str_length) {
    index = edit_str_length - 1;
  }

  if (index != edit_cursor_index) {
    edit_cursor_index = index;
    edit_cursor_changed();
    changed = 1;
  }

  return changed;
}

static void edit_changed(void)
{
  edit_flags.changed = 1;
  edit_cursor_changed();
}

static void edit_shift_right(void)
{
  int n = edit_str_length - 1;

  //edit_str[n] = 0;
  n--;

  while (n > edit_cursor_index) {
    edit_str[n] = edit_str[n - 1];
    n--;
  }

  edit_changed();
}

static void edit_shift_left(void)
{
  const int last = edit_str_length - 3;
  int n = edit_cursor_index;

  while (n < last) {
    edit_str[n] = edit_str[n + 1];
    n++;
  }

  edit_str[n] = 32;
  //edit_str[n + 1] = 0;
  edit_changed();
}

static void edit_str_prep(void)
{
  const int last = edit_str_length - 1;

  edit_str[last] = 0;

  for (int n = 0; n < last; n++) {
    char c = edit_str[n];
    if (!c) {
      edit_str[n] = 32;
    }
  }
}

int UI_edit_text_cursor_moved(void)
{
  const int is_moved = edit_flags.cursor_moved;

  edit_flags.cursor_moved = 0;

  return is_moved;
}

void UI_edit_text_start(char *str, const short length)
{
  edit_str = str;
  edit_str_length = length;
  edit_flags.insert_mode = 1;
  edit_str_prep();
}

void UI_edit_text_stop(void)
{
  edit_cursor_blink_task_stop();
}

int UI_edit_text_changed(void)
{
  const int is_changed = edit_flags.changed;
  
  edit_flags.changed = 0;

  return is_changed;
}

void UI_edit_text_set_changed(void)
{
  edit_changed();
}

short UI_edit_text_cursor_index(void)
{
  return edit_cursor_index;
}

int UI_edit_text_cursor_blink(void)
{
  return edit_flags.cursor_blink;
}

int UI_edit_text_insert_mode(void)
{
  return edit_flags.insert_mode;
}

static void edit_cursor_left(const short area_id, const short ui_id)
{
  if (edit_cursor_index > 0) {
    edit_cursor_index--;
    edit_cursor_changed();
  }

  UI_area_set_handled(area_id);
}

static void edit_cursor_handled(const short area_id)
{
  edit_cursor_changed();
  UI_area_set_handled(area_id);
}

static void edit_cursor_right(const short area_id, const short ui_id)
{
  if (edit_cursor_index < edit_str_length) {
    edit_cursor_index++;
  }

  edit_cursor_handled(area_id);
}

static void edit_cursor_home(const short area_id, const short ui_id)
{
  edit_cursor_index = 0;
  edit_cursor_handled(area_id);
}

static void edit_cursor_end(const short area_id, const short ui_id)
{
  edit_cursor_index = edit_str_length;
  edit_cursor_handled(area_id);
}

static void edit_delete_right(const short area_id, const short ui_id)
{
  edit_shift_left();
  edit_cursor_handled(area_id);
}

static void edit_delete_left(const short area_id, const short ui_id)
{
  edit_cursor_left(area_id, ui_id);
  edit_delete_right(area_id, ui_id);
}

static void edit_insert_toggle(const short area_id, const short ui_id)
{
  edit_flags.insert_mode = !edit_flags.insert_mode;
  edit_cursor_changed();
}

static short get_edit_key_map(void)
{
  if (!edit_key_map) {
    edit_key_map = Key_Map_create();
    Key_Map_add(edit_key_map, Key_Action_create(LEFT_KEY, edit_cursor_left, 0));
    Key_Map_add(edit_key_map, Key_Action_create(RIGHT_KEY, edit_cursor_right, 0));
    Key_Map_add(edit_key_map, Key_Action_create(HOME_KEY, edit_cursor_home, 0));
    Key_Map_add(edit_key_map, Key_Action_create(END_KEY, edit_cursor_end, 0));
    Key_Map_add(edit_key_map, Key_Action_create(BKSPC_KEY, edit_delete_left, 0));
    Key_Map_add(edit_key_map, Key_Action_create(DEL_KEY, edit_delete_right, 0));
    Key_Map_add(edit_key_map, Key_Action_create(INS_KEY, edit_insert_toggle, 0));
  }

  return edit_key_map;
}

static void edit_add_char(const char key, const short area_id, const short ui_id)
{
  if (edit_flags.insert_mode) {
    edit_shift_right();
  }

  edit_str[edit_cursor_index] = key;
  edit_cursor_right(area_id, ui_id);
  edit_changed();
}

static void edit_key_change(const short area_id, const short ui_id, const char first_key, const char last_key)
{
  const int key = UI_area_active_key();

  if ((key >= first_key) && (key <= last_key)) {
    edit_add_char(key, area_id, ui_id);
  }
  else {
    Key_Map_action(get_edit_key_map(), key, area_id, ui_id);
  }
}

void UI_edit_text_number_key_change(const short area_id, const short ui_id)
{
  edit_key_change(area_id, ui_id, '-', '9');
}

void UI_edit_text_key_change(const short area_id, const short ui_id)
{
  edit_key_change(area_id, ui_id, 32, 126);
}

