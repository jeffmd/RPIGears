// ui_text.c

#include <stdio.h>
#include <stdint.h>

#include "id_plug.h"
#include "static_array.h"
#include "connector.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "text.h"

typedef struct {
  uint8_t active;
  ID_t text;
  ID_t area;
} UI_Text;

#define UI_TEXT_MAX_COUNT 50

static UI_Text ui_texts[UI_TEXT_MAX_COUNT];
static ID_t next_deleted_ui_text;

static ID_t area_connector;
static ID_t ui_text_class;

static inline ID_t find_deleted_ui_text(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_ui_text, ui_texts, UI_TEXT_MAX_COUNT, UI_Text, "UI text");
}

static UI_Text *get_ui_text(ID_t id)
{
  if ((id < 0) | (id >= UI_TEXT_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI text id, using default id: 0\n");
  }
    
  return ui_texts + id;
}

static void ui_text_init(UI_Text *ui_text)
{
  ui_text->area = 0;
}

/*
static void ui_text_enter(const ID_t source_id, const ID_t destination_id)
{
  //printf("enter text area %i\n", source_id);
}

static void ui_text_leave(const ID_t source_id, const ID_t destination_id)
{
  //printf("leave text area %i\n", source_id);
}
*/

static ID_t get_text(UI_Text *ui_text)
{
  if (!ui_text->text) {
    ui_text->text = Text_create();
  }

  return ui_text->text;
}

static void update_dimensions(UI_Text *ui_text, const ID_t source_id)
{
  if (ui_text->area != source_id)
  {
    short size[2];

    ui_text->area = source_id;
    UI_area_size(source_id, size);
    Text_set_offset(get_text(ui_text), size[0], size[1]);
    //printf("update text offset %i\n", text->text_id);
  }
}

static void ui_text_draw(const ID_t source_id, const ID_t destination_id)
{
  UI_Text *const ui_text = get_ui_text(destination_id);

  update_dimensions(ui_text, source_id);
  Text_draw(get_text(ui_text));
  //printf("draw text area %i\n", source_id);
}

static void area_clear(const ID_t id)
{
  UI_Text *const ui_text = get_ui_text(id);
  ui_text->area = 0;
}

static void update_area_size(UI_Text *ui_text, const ID_t area_id)
{
  int extent[2];

  Text_extent(get_text(ui_text), extent);
  UI_area_set_size(area_id, extent[0], extent[1]);
  //printf("text area size x: %i, y: %i\n", extent[0], extent[1]); 
}

static void ui_text_area_attach(const ID_t source_id, const ID_t destination_id)
{
  //printf("attach text area %i\n", source_id);
  UI_Text *const ui_text = get_ui_text(destination_id);
  update_area_size(ui_text, source_id);
  area_clear(destination_id);
}

static void ui_text_area_resize(const ID_t source_id, const ID_t destination_id)
{
  //printf("resize text area %i\n", source_id);
  area_clear(destination_id);
}

static ID_t get_ui_text_class(void)
{
  if(!ui_text_class) {
    ui_text_class = Connector_register_class("ui_text");
  }

  return ui_text_class;
}

static ID_t get_area_connector(void)
{
  if (!area_connector) {
    printf("create ui text area connector: ");
    area_connector = UI_area_connector(get_ui_text_class());
    //UI_area_connect_enter(area_connector, ui_text_enter);
    //UI_area_connect_leave(area_connector, ui_text_leave);
    UI_area_connect_draw(area_connector, ui_text_draw);
    UI_area_connect_resize(area_connector, ui_text_area_resize);
    UI_area_connect_attach(area_connector, ui_text_area_attach);
  }

  return area_connector;
}

static Plug_t get_ui_text_plug(const ID_t ui_text_id)
{
  return Connector_plug(get_area_connector(), ui_text_id);
}

Plug_t UI_text_create(const char *str)
{
  const ID_t id = find_deleted_ui_text();
  UI_Text *const ui_text = get_ui_text(id);
  ui_text->active = 1;
  ui_text_init(ui_text);

  Text_add(get_text(ui_text), 0, 0, str);

  return get_ui_text_plug(id);
}

ID_t UI_text_text_id(const ID_t id)
{
  UI_Text *const ui_text = get_ui_text(id);
  return get_text(ui_text);
}

void UI_text_add(const ID_t id, const char *str)
{
  UI_Text *const ui_text = get_ui_text(id);
  Text_add(get_text(ui_text), 0, 0, str);
}

