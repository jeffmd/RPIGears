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
  uint8_t sliding:1;
  short area;
  short cur_travel;
  short max_travel;
  int widget_handle;
  float normal_scale[2];
  float normal_offset[2];
  float select_scale[2];
  float select_offset[2];
  float select2_scale[2];
  float select2_offset[2];
} UI_Slider;

#define UI_SLIDER_MAX_COUNT 50

static UI_Slider ui_sliders[UI_SLIDER_MAX_COUNT];
static short next_deleted_ui_slider;

static short area_connector;
static short ui_slider_class;
static short ui_slider_key_map;

static inline short find_deleted_ui_slider(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_ui_slider, ui_sliders, UI_SLIDER_MAX_COUNT, UI_Slider, "UI slider");
}

static UI_Slider *get_ui_slider(short id)
{
  if ((id < 0) | (id >= UI_SLIDER_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI slider id, using default id: 0\n");
  }
    
  return ui_sliders + id;
}

static void ui_slider_init(UI_Slider *ui_slider)
{
  ui_slider->cur_travel = 0;
  ui_slider->max_travel = 100;
  ui_slider->sliding = 0;
}

static short get_ui_slider_class(void)
{
  if(!ui_slider_class) {
    ui_slider_class = Connector_register_class("ui_slider");
  }

  return ui_slider_class;
}

static void update_dimensions(UI_Slider *ui_slider, const short area_id)
{
  if (ui_slider->area != area_id)
  {
    short size[2];

    ui_slider->area = area_id;
    UI_area_size(area_id, size);

    ui_slider->normal_scale[0] = 2.0f * size[0] - 2.0f;
    ui_slider->normal_scale[1] = 2.0f * size[1] - 2.0f;
    ui_slider->normal_offset[0] = -size[0];
    ui_slider->normal_offset[1] = -size[1];

    ui_slider->select_scale[0] = 2.0f * size[0] - 6.0f;
    ui_slider->select_scale[1] = 2.0f * size[1] - 6.0f;
    ui_slider->select_offset[0] = -size[0] + 2.0f;
    ui_slider->select_offset[1] = -size[1] + 2.0f;

    ui_slider->select2_scale[0] = 2.0f * size[0] - 10.0f;
    ui_slider->select2_scale[1] = 2.0f * size[1] - 10.0f;
    ui_slider->select2_offset[0] = -size[0] + 4.0f;
    ui_slider->select2_offset[1] = -size[1] + 4.0f;
  }
}

static void ui_slider_draw(const short area_id, const short ui_slider_id)
{
  UI_Slider *const ui_slider = get_ui_slider(ui_slider_id);

  update_dimensions(ui_slider, area_id);
  UI_icon_draw_box(ui_slider->normal_scale[0], ui_slider->normal_scale[1], ui_slider->normal_offset[0], ui_slider->normal_offset[1]);

  if (UI_area_is_active(area_id)) {
    UI_icon_draw_box(ui_slider->select_scale[0], ui_slider->select_scale[1], ui_slider->select_offset[0], ui_slider->select_offset[1]);

    if (ui_slider->sliding) {
      UI_icon_draw_box(ui_slider->select2_scale[0], ui_slider->select2_scale[1], ui_slider->select2_offset[0], ui_slider->select2_offset[1]);
    }
  }
}

static void area_clear(const short id)
{
  UI_Slider *const ui_slider = get_ui_slider(id);
  ui_slider->area = 0;
}

static void update_area_size(UI_Slider *ui_slider, const short area_id)
{
  UI_area_set_size(area_id, 10, 20);
}

static void ui_slider_area_attach(const short area_id, const short ui_slider_id)
{
  UI_Slider *const ui_slider = get_ui_slider(ui_slider_id);
  update_area_size(ui_slider, area_id);
  area_clear(ui_slider_id);
}

static void ui_slider_area_resize(const short area_id, const short ui_slider_id)
{
  area_clear(ui_slider_id);
}

static void ui_slider_start_drag(const short area_id, const short ui_slider_id)
{
  UI_area_drag_start();
  UI_area_set_handled(area_id);
  UI_area_set_locked(area_id);
  get_ui_slider(ui_slider_id)->sliding = 1;
}

static void ui_slider_end_drag(const short area_id, const short ui_slider_id)
{
  UI_area_drag_end();
  UI_area_set_handled(area_id);
  UI_area_set_unlocked(area_id);
  get_ui_slider(ui_slider_id)->sliding = 0;
}

static short get_ui_slider_key_map(void)
{
  if (!ui_slider_key_map) {
    ui_slider_key_map = Key_Map_create();
    Key_Map_add(ui_slider_key_map, Key_Action_create(LEFT_BUTTON, ui_slider_start_drag, 0));
    Key_Map_add(ui_slider_key_map, Key_Action_create(LEFT_BUTTON_RELEASE, ui_slider_end_drag, 0));
  }

  return ui_slider_key_map;
}

static void ui_slider_area_key_change(const short area_id, const short ui_slider_id)
{
  Key_Map_action(get_ui_slider_key_map(), UI_area_active_key(), area_id, ui_slider_id);
}

static void ui_slider_area_pointer_drag(const short area_id, const short ui_slider_id)
{
  short delta = -UI_area_drag_delta_y();

  if (delta != 0) {
    UI_Slider *ui_slider = get_ui_slider(ui_slider_id);
    short travel = ui_slider->cur_travel + delta;

    if (travel > ui_slider->max_travel) {
      travel = ui_slider->max_travel;
    }
    else if (travel < 0) {
      travel = 0;
    }

    delta = travel - ui_slider->cur_travel;

    if (delta != 0) {
      ui_slider->cur_travel = travel;
      UI_area_change_offset(area_id, 0, delta);
      UI_widget_changed(ui_slider->widget_handle, ui_slider_id);
    }
  }
}

static short get_area_connector(void)
{
  if (!area_connector) {
    printf("create ui slider area connector: ");
    area_connector = UI_area_connector(get_ui_slider_class());
    UI_area_connect_draw(area_connector, ui_slider_draw);
    UI_area_connect_resize(area_connector, ui_slider_area_resize);
    UI_area_connect_attach(area_connector, ui_slider_area_attach);
    UI_area_connect_key_change(area_connector, ui_slider_area_key_change);
    UI_area_connect_pointer_drag(area_connector, ui_slider_area_pointer_drag);
  }

  return area_connector;
}

static int get_ui_slider_area_handle(const short ui_slider_id)
{
  return Connector_handle(get_area_connector(), ui_slider_id);
}

void UI_slider_connect_widget(const short ui_slider_id, const int handle)
{
  UI_Slider *const ui_slider = get_ui_slider(ui_slider_id);
  ui_slider->widget_handle = handle;
}

int UI_slider_create(const int handle)
{
  const short id = find_deleted_ui_slider();
  UI_Slider *const ui_slider = get_ui_slider(id);
  ui_slider->active = 1;
  ui_slider_init(ui_slider);
  UI_slider_connect_widget(id, handle);

  return get_ui_slider_area_handle(id);
}
