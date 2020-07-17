// ui_checkbox.c

#include <stdio.h>
#include <stdint.h>

#include "connector.h"
#include "ui_area.h"
#include "ui_area_action.h"
#include "static_array.h"
#include "text.h"

typedef struct {
  uint8_t active;
  short text;
  short area;
} UI_CheckBox;

#define UI_CHECKBOX_MAX_COUNT 50

static UI_CheckBox ui_checkboxes[UI_CHECKBOX_MAX_COUNT];
static short next_deleted_ui_checkbox;

static short area_connector;
static short ui_checkbox_class;

static inline short find_deleted_ui_checkbox(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_ui_checkbox, ui_checkboxes, UI_CHECKBOX_MAX_COUNT, UI_CheckBox, "UI text");
}

static UI_CheckBox *get_ui_checkbox(short id)
{
  if ((id < 0) | (id >= UI_CHECKBOX_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI text id, using default id: 0\n");
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
    Text_set_offset(get_text(ui_checkbox), size[0], size[1]);
    //printf("update checkbox offset %i\n", text->text_id);
  }
}

static void ui_checkbox_draw(const short source_id, const short destination_id)
{
  UI_CheckBox *const ui_checkbox = get_ui_checkbox(destination_id);

  update_dimensions(ui_checkbox, source_id);
  Text_draw(get_text(ui_checkbox));
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
  UI_area_set_size(area_id, extent[0], extent[1]);
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
  }

  return area_connector;
}

static int get_ui_checkbox_handle(const short ui_checkbox_id)
{
  return Connector_handle(get_area_connector(), ui_checkbox_id);
}

int UI_checkbox_create(const char *str)
{
  const short id = find_deleted_ui_checkbox();
  UI_CheckBox *const ui_checkbox = get_ui_checkbox(id);
  ui_checkbox->active = 1;
  ui_checkbox_init(ui_checkbox);
  Text_add(get_text(ui_checkbox), 0, 0, str);

  return get_ui_checkbox_handle(id);
}
