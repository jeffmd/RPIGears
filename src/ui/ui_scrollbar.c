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
#include "ui_icon.h"

typedef struct {
  uint8_t active;
  uint8_t scrolling:1;
  short scroll_area;
} UI_Scrollbar;

#define UI_SCROLLBAR_MAX_COUNT 50

static UI_Scrollbar ui_scrollbars[UI_SCROLLBAR_MAX_COUNT];
static short next_deleted_ui_scrollbar;

static short area_connector;
static short ui_scrollbar_class;
static short ui_scrollbar_key_map;
static short delta_xy;

static inline short find_deleted_ui_scrollbar(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_ui_scrollbar, ui_scrollbars, UI_SCROLLBAR_MAX_COUNT, UI_Scrollbar, "UI scrollbar");
}

static UI_Scrollbar *get_ui_scrollbar(short id)
{
  if ((id < 0) | (id >= UI_SCROLLBAR_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI scrollbar id, using default id: 0\n");
  }
    
  return ui_scrollbars + id;
}

static void ui_scrollbar_init(UI_Scrollbar *ui_scrollbar)
{

}

static short get_ui_scrollbar_class(void)
{
  if(!ui_scrollbar_class) {
    ui_scrollbar_class = Connector_register_class("ui_scrollbar");
  }

  return ui_scrollbar_class;
}

static short get_area_connector(void)
{
  if (!area_connector) {
    printf("create ui scrollbar area connector: ");
    area_connector = UI_area_connector(get_ui_scrollbar_class());
    //UI_area_connect_draw(area_connector, ui_scrollbar_draw);
    //UI_area_connect_resize(area_connector, ui_scrollbar_area_resize);
    //UI_area_connect_attach(area_connector, ui_scrollbar_area_attach);
    //UI_area_connect_key_change(area_connector, ui_scrollbar_area_key_change);
    //UI_area_connect_pointer_drag(area_connector, ui_scrollbar_area_pointer_drag);
  }

  return area_connector;
}

static int get_ui_scrollbar_area_handle(const short ui_scrollbar_id)
{
  return Connector_handle(get_area_connector(), ui_scrollbar_id);
}

int UI_scrollbar_create(const short scroll_area)
{
  const short id = find_deleted_ui_scrollbar();
  UI_Scrollbar *const ui_scrollbar = get_ui_scrollbar(id);
  ui_scrollbar->active = 1;
  ui_scrollbar_init(ui_scrollbar);

  ui_scrollbar->scroll_area = scroll_area;

  return get_ui_scrollbar_area_handle(id);
}
