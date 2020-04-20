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
  short text_id;
  short area_id;
  short area_handler_id;
} UI_Text;

#define UI_TEXT_MAX_COUNT 50

static UI_Text texts[UI_TEXT_MAX_COUNT];
static short next_deleted_text;

static short area_action_table;
//static short text_action_table;

static inline short find_deleted_text_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_text, texts, UI_TEXT_MAX_COUNT, UI_Text, "UI text");
}

static UI_Text *get_text(short id)
{
  if ((id < 0) | (id >= UI_TEXT_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI text id, using default id: 0\n");
  }
    
  return texts + id;
}

static void text_init(UI_Text *text)
{
  text->area_id = 0;
}

static void ui_text_enter(const short source_id, const short destination_id)
{
  printf("enter text area %i\n", source_id);
}

static void ui_text_leave(const short source_id, const short destination_id)
{
  printf("leave text area %i\n", source_id);
}

static void update_dimensions(UI_Text *text, const short source_id)
{
  if (text->area_id != source_id)
  {
    text->area_id = source_id;
  }
}

static void ui_text_draw(const short source_id, const short destination_id)
{
  UI_Text *const text = get_text(destination_id);

  update_dimensions(text, source_id);
  Text_draw(text->text_id);
  //printf("draw text area %i\n", source_id);
}

static void ui_text_area_attach(const short source_id, const short destination_id)
{
  printf("attach text area %i\n", source_id);
}

static void ui_text_resize(const short source_id, const short destination_id)
{
  printf("resize text area %i\n", source_id);
}

static short get_area_action_table(void)
{
  if (!area_action_table) {
    printf("create ui text area action table: ");
    area_action_table = UI_area_create_action_table();
    UI_area_action_set_enter(area_action_table, ui_text_enter);
    UI_area_action_set_leave(area_action_table, ui_text_leave);
    UI_area_action_set_draw(area_action_table, ui_text_draw);
    UI_area_action_set_resize(area_action_table, ui_text_resize);
    UI_area_action_set_attach(area_action_table, ui_text_area_attach);
  }

  return area_action_table;
}

short UI_text_create(void)
{
  const short id = find_deleted_text_id();
  UI_Text *const text = get_text(id);
  text->active = 1;
  text_init(text);

  return id;
}

short UI_text_area_handler(const short ui_text_id)
{
  UI_Text *const text = get_text(ui_text_id);
  if (!text->area_handler_id) {
    text->area_handler_id = Handler_create(ui_text_id, get_area_action_table());
  }

  return text->area_handler_id;
}

void UI_text_set_text_id(const short id, const short text_id)
{
  UI_Text *const text = get_text(id);
  text->text_id = text_id;
}
