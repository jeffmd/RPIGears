// ui_text.c

#include <stdio.h>
#include <stdint.h>

#include "action_table.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "handler.h"
#include "static_array.h"
#include "text.h"

typedef struct {
  uint8_t active;
  short text;
  short area;
} UI_Text;

#define UI_TEXT_MAX_COUNT 50

static UI_Text ui_texts[UI_TEXT_MAX_COUNT];
static short next_deleted_ui_text;

static short area_action_table;
static short ui_text_class;

static inline short find_deleted_ui_text(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_ui_text, ui_texts, UI_TEXT_MAX_COUNT, UI_Text, "UI text");
}

static UI_Text *get_ui_text(short id)
{
  if ((id < 0) | (id >= UI_TEXT_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI text id, using default id: 0\n");
  }
    
  return ui_texts + id;
}

static void text_init(UI_Text *ui_text)
{
  ui_text->area = 0;
}

static void ui_text_enter(const short source_id, const short destination_id)
{
  printf("enter text area %i\n", source_id);
}

static void ui_text_leave(const short source_id, const short destination_id)
{
  printf("leave text area %i\n", source_id);
}

static short get_text(UI_Text *ui_text)
{
  if (!ui_text->text) {
    ui_text->text = Text_create();
  }

  return ui_text->text;
}

static void update_dimensions(UI_Text *ui_text, const short source_id)
{
  if (ui_text->area != source_id)
  {
    int size[2];

    ui_text->area = source_id;
    UI_area_size(source_id, size);
    Text_set_offset(get_text(ui_text), size[0], size[1]);
    //printf("update text offset %i\n", text->text_id);
  }
}

static void ui_text_draw(const short source_id, const short destination_id)
{
  UI_Text *const ui_text = get_ui_text(destination_id);

  update_dimensions(ui_text, source_id);
  Text_draw(get_text(ui_text));
  //printf("draw text area %i\n", source_id);
}

static void area_clear(const short id)
{
  UI_Text *const ui_text = get_ui_text(id);
  ui_text->area = 0;
}

static void update_area_size(UI_Text *ui_text, const short area_id)
{
  int extent[2];

  Text_extent(get_text(ui_text), extent);
  UI_area_set_size(area_id, extent[0], extent[1]);
  printf("text area size x: %i, y: %i\n", extent[0], extent[1]); 
}

static void ui_text_area_attach(const short source_id, const short destination_id)
{
  printf("attach text area %i\n", source_id);
  UI_Text *const ui_text = get_ui_text(destination_id);
  update_area_size(ui_text, source_id);
  area_clear(destination_id);
}

static void ui_text_area_resize(const short source_id, const short destination_id)
{
  printf("resize text area %i\n", source_id);
  area_clear(destination_id);
}

static short get_class(void)
{
  if(!ui_text_class) {
    ui_text_class = Action_table_register_class("ui_text");
  }

  return ui_text_class;
}

static short get_area_action_table(void)
{
  if (!area_action_table) {
    printf("create ui text area action table: ");
    area_action_table = UI_area_create_action_table(get_class());
    UI_area_action_set_enter(area_action_table, ui_text_enter);
    UI_area_action_set_leave(area_action_table, ui_text_leave);
    UI_area_action_set_draw(area_action_table, ui_text_draw);
    UI_area_action_set_resize(area_action_table, ui_text_area_resize);
    UI_area_action_set_attach(area_action_table, ui_text_area_attach);
  }

  return area_action_table;
}

static int get_handler(const short ui_text_id)
{
  return Handler_create(ui_text_id, get_area_action_table());
}

int UI_text_create(void)
{
  const short id = find_deleted_ui_text();
  UI_Text *const ui_text = get_ui_text(id);
  ui_text->active = 1;
  text_init(ui_text);

  return get_handler(id);
}

short UI_text_text_id(const short id)
{
  UI_Text *const ui_text = get_ui_text(id);
  return get_text(ui_text);
}

void UI_text_add(const short id, const char *str)
{
  UI_Text *const ui_text = get_ui_text(id);
  Text_add(get_text(ui_text), 0, 0, str);
}

