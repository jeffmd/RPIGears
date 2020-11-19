// ui_area.c - user interface area

#include <stdio.h>
#include <stdint.h>

#include "window.h"
#include "static_array.h"
#include "connector.h"
#include "key_input.h"
#include "ui_layout.h"

typedef struct {
  uint8_t active;
  uint8_t modid;
  uint8_t parent_modid;
  // flags
  unsigned char hide:1;
  unsigned char visible:1;
  //unsigned selectable:1;
  unsigned char handled:1;

  // area id links
  short parent;
  short first_child;
  short last_child;
  short next_sibling;
  short prev_sibling;

  short layout;

  short layout_pos[2];
  short rel_pos[2];
  short size[2];
  short abs_pos[2];
  short vis_pos[4];

  // Connector handle
  int handle;
} UI_Area;

enum Events {
  OnAttach,
  OnEnter,
  OnLeave,
  OnMove,
  OnResize,
  OnDraw,
  OnKeyChange,
  OnPointerMove,
  OnPointerDrag,
  EventsMax
};

#define UI_AREA_MAX_COUNT 100

static UI_Area areas[UI_AREA_MAX_COUNT];
static short next_deleted_area;

static short root_area;
static short active_area;
static short locked_area;
static int active_key;

static short pointer_x;
static short pointer_y;
static short old_y;
static short old_x;
static short drag_active;

static short ui_area_class;

static short get_active_area(void)
{
  if (active_area)
    return active_area;
  else
    return root_area;
}

static void clear_active_area(void)
{
  active_area = 0;
}

static inline short find_deleted_area(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_area, areas, UI_AREA_MAX_COUNT, UI_Area, "UI area");
}

static UI_Area *get_area(short id)
{
  if ((id < 0) | (id >= UI_AREA_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI Area id, using default id: 0\n");
  }
    
  return areas + id;
}

static void area_changed(UI_Area *area)
{
  area->modid++;
}

static void area_init(UI_Area *area)
{
  area->next_sibling = 0;
  area->prev_sibling = 0;
  area->parent = 0;
  area->first_child = 0;
  area->last_child = 0;
  area->handle = 0;
  area_changed(area);
}

static void clip_pos(const short clip_pos[4], short pos[2])
{
  if (pos[0] < clip_pos[0]) {
    pos[0] = clip_pos[0];
  }
  else {
    if (pos[0] > clip_pos[2])
      pos[0] = clip_pos[2];
  }

  if (pos[1] < clip_pos[1]) {
    pos[1] = clip_pos[1];
  }
  else {
    if (pos[1] > clip_pos[3])
      pos[1] = clip_pos[3];
  }
}

static void update_vis_pos(UI_Area *area)
{
  const short parent_id = area->parent;
  short *pos = area->vis_pos;

  pos[0] = area->rel_pos[0];
  pos[1] = area->rel_pos[1];

  pos[2] = pos[0] + area->size[0];
  pos[3] = pos[1] + area->size[1];

  if (parent_id) {
    UI_Area *parent_area = get_area(parent_id);
    pos[0] += parent_area->abs_pos[0];
    pos[2] += parent_area->abs_pos[0];
    pos[1] += parent_area->abs_pos[1];
    pos[3] += parent_area->abs_pos[1];

    if (parent_area->layout) {
      // activate layout
      pos[0] += area->layout_pos[0];
      pos[2] += area->layout_pos[0];
      pos[1] += area->layout_pos[1];
      pos[3] += area->layout_pos[1];
    }

    area->abs_pos[0] = pos[0];
    area->abs_pos[1] = pos[1];
    // clip pos to parent vis_pos
    clip_pos(parent_area->vis_pos, pos);
    clip_pos(parent_area->vis_pos, pos + 2);

    area->parent_modid = parent_area->modid;
  }
  
  area_changed(area);
}

static int area_inside(const short pos[4], const int x, const int y)
{
  int is_inside = 0;

  if ( (x >= pos[0]) && (x <= pos[2]) ) {
    if ((y >= pos[1]) && (y <= pos[3]) ) {
      is_inside = 1;
    }
  }

  return is_inside;
}

static int child_inside_parent(UI_Area *area)
{
  int is_inside = 0;
  short *pos = area->vis_pos;

  if ( (pos[2] - pos[0]) && (pos[3] - pos[1]) ) {
    is_inside = 1;
  }
  
  return is_inside;
} 

static void update_visibility(UI_Area *area)
{
  update_vis_pos(area);

  int new_visible;
 
  if (!area->hide) {
    new_visible = child_inside_parent(area);
  }
  else {
    new_visible = !area->hide;
  }

  if (new_visible != area->visible) {
    area->visible = new_visible;
    clear_active_area();
  }
}

static int is_visible(UI_Area *area)
{
  if (area->parent) {
    if (area->parent_modid != get_area(area->parent)->modid) {
      update_visibility(area);
      //printf("udate visibility\n");
    }
  }

  return area->visible;
}

short UI_area_create(void)
{
  const int id = find_deleted_area();
  UI_Area *const area = get_area(id);
  area->active = 1;
  area_init(area);

  return id;
}

static void parent_update_layout(UI_Area *parent, const short child)
{
  if (parent->layout) {
    // update layout of child area
    UI_Layout_update(parent->layout, child);
    // mark parent as changed
    area_changed(parent);
  }
}

void UI_area_remove_parent(const short area_id)
{
  UI_Area * const area = get_area(area_id);

  if (area->parent) {
    UI_Area *parent = get_area(area->parent);

    if (parent->last_child == area_id) {
      parent->last_child = area->prev_sibling;
    }

    if (parent->first_child == area_id) {
      parent->first_child = area->next_sibling;
    }

    short update_sibling_id = 0;

    if (area->prev_sibling) {
      UI_Area *sibling = get_area(area->prev_sibling);
      sibling->next_sibling = area->next_sibling;
      update_sibling_id = area->prev_sibling;
    }

    if (area->next_sibling) {
      UI_Area *sibling = get_area(area->next_sibling);
      sibling->prev_sibling = area->prev_sibling;
      update_sibling_id = area->next_sibling;
    }

    parent_update_layout(parent, update_sibling_id);
  }

  area->next_sibling = 0;
  area->prev_sibling = 0;
  area->parent = 0;
}

void UI_area_add(const short parent_id, const short child_id)
{
  UI_Area * child = get_area(child_id);

  if (child->parent != parent_id) {
    UI_area_remove_parent(child_id);
    UI_Area * parent = get_area(parent_id);
    child->prev_sibling = parent->last_child;

    if (parent->last_child) {
      UI_Area * old_last_child = get_area(parent->last_child);
      old_last_child->next_sibling = child_id;
    }

    child->parent = parent_id;
    parent->last_child = child_id;

    if (!parent->first_child) {
      parent->first_child = child_id;
    }

    parent_update_layout(parent, child_id);
  }

  update_visibility(child);
}

void UI_area_set_active(const short area_id)
{
  if (active_area != area_id) {
    UI_Area *area = get_area(active_area);
    Connector_handle_execute(area->handle, OnLeave, active_area);
    active_area = area_id;
    area = get_area(area_id);
    Connector_handle_execute(area->handle, OnEnter, area_id);
  }
}

int UI_area_is_active(const short area_id)
{
  //printf("area_id=%i active_area=%i\n", area_id, active_area);
  return (area_id == active_area);
}

void UI_area_set_root(const short area_id)
{
  root_area = area_id;
}

void UI_area_set_offset(const short area_id, const short x, const short y)
{
  UI_Area * const area = get_area(area_id);
  area->rel_pos[0] = x;
  area->rel_pos[1] = y;
  update_visibility(area);
  Connector_handle_execute(area->handle, OnMove, area_id);
}

void UI_area_offset(const short area_id, short pos[2])
{
  UI_Area * const area = get_area(area_id);
  pos[0] = area->rel_pos[0];
  pos[1] = area->rel_pos[1];
}

void UI_area_set_layout_position(const short area_id, const short x, const short y)
{
  UI_Area * const area = get_area(area_id);
  area->layout_pos[0] = x;
  area->layout_pos[1] = y;
  update_visibility(area);
  Connector_handle_execute(area->handle, OnMove, area_id);
}

void UI_area_layout_position(const short area_id, short pos[2])
{
  UI_Area * const area = get_area(area_id);
  pos[0] = area->layout_pos[0];
  pos[1] = area->layout_pos[1];
}

void UI_area_set_size(const short area_id, const short width, const short height)
{
  UI_Area * const area = get_area(area_id);
  area->size[0] = width;
  area->size[1] = height;
  update_visibility(area);
  Connector_handle_execute(area->handle, OnResize, area_id);
}

void UI_area_size(const short area_id, short size[2])
{
  UI_Area * const area = get_area(area_id);
  size[0] = area->size[0];
  size[1] = area->size[1];
}

void UI_area_offset_size(const short area_id, short offset_size[2])
{
  UI_Area * const area = get_area(area_id);
  offset_size[0] = area->rel_pos[0] + area->size[0];
  offset_size[1] = area->rel_pos[1] + area->size[1];
}

short UI_area_offset_size_x(const short area_id)
{
  UI_Area * const area = get_area(area_id);

  return area->rel_pos[0] + area->size[0];
}

short UI_area_offset_size_y(const short area_id)
{
  UI_Area * const area = get_area(area_id);

  return area->rel_pos[1] + area->size[1];
}

static short area_find(short area_id, const int check_sibling, const int x, const int y)
{
  short newhit = 0;

  while (area_id) {
    UI_Area *area = get_area(area_id);

    if (is_visible(area) && area_inside(area->vis_pos, x, y)) {
      // check children starting with last
      newhit = area_find(area->last_child, 1, x, y);
      if (!newhit) {
        newhit = area_id;
      }
      area_id = 0;
    }
    else {
      area_id = check_sibling ? area->prev_sibling : area->parent;
    }
  }

  return newhit;
}

static void area_pointer_moved(const short area_id)
{
  UI_Area * const area = get_area(area_id);
  Connector_handle_execute(area->handle, OnPointerMove, area_id);

  if (drag_active) {
    Connector_handle_execute(area->handle, OnPointerDrag, area_id);
    old_y = pointer_y;
    old_x = pointer_x;
  }

}

static void update_active_area(void)
{
  if (!locked_area) {
    UI_area_set_active(area_find(get_active_area(), 0, pointer_x, pointer_y));
  }
  
  area_pointer_moved(get_active_area());
}

void UI_area_set_locked(const short area_id)
{
  locked_area = area_id;
  UI_area_set_active(area_id);
}

void UI_area_set_unlocked(const short area_id)
{
  if (locked_area == area_id) {
    locked_area = 0;
    update_active_area();
  }
}

int UI_area_locked_hit(void)
{
  int is_hit;

  if (locked_area) {
    is_hit = area_inside(get_area(locked_area)->vis_pos, pointer_x, pointer_y);
  }
  else {
    is_hit = 0;
  }

  return is_hit;
}

void UI_area_select_active(const int key, const int x, const int y)
{
  active_key = key;
  pointer_x = x;
  pointer_y = y;

  update_active_area();
}

void UI_area_key_change(const int key)
{
  int handled = 0;
  short area_id = get_active_area();
  active_key = key;

  while (area_id) {
    UI_Area *area = get_area(area_id);
    area->handled = 0;
    Connector_handle_execute(area->handle, OnKeyChange, area_id);
    handled = area->handled;

    if (handled) {
      area_id = 0;
    }
    else {
      area_id = area->parent;
    }
  }

  if (!handled) {
    Key_input_action(key);
  }
}

static short get_class(void)
{
  if (!ui_area_class) {
    ui_area_class = Connector_register_class("ui_area");
  }

  return ui_area_class;
}

short UI_area_connector(const short destination_class)
{
  const short connector_id = Connector_create(get_class(), destination_class, EventsMax);

  return connector_id;
}

void UI_area_connect_enter(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnEnter, action);
}

void UI_area_connect_leave(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnLeave, action);
}

void UI_area_connect_draw(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnDraw, action);
}

void UI_area_connect_resize(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnResize, action);
}

void UI_area_connect_move(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnMove, action);
}

void UI_area_connect_attach(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnAttach, action);
}

void UI_area_connect_key_change(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnKeyChange, action);
}

void UI_area_connect_pointer_move(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnPointerMove, action);
}

void UI_area_connect_pointer_drag(const short connector_id, ActionFn action)
{
  Connector_set_action(connector_id, OnPointerDrag, action);
}

void UI_area_connect(const short area_id, const int handle)
{
  UI_Area * const area = get_area(area_id);
  area->handle = handle;
  Connector_handle_execute(handle, OnAttach, area_id);
}

short UI_area_add_handle(const short parent_id, const int handle, const int x, const int y)
{
  const short child_area = UI_area_create();
  UI_area_connect(child_area, handle);
  UI_area_set_offset(child_area, x, y);
  UI_area_add(parent_id, child_area);

  return child_area;
}

void UI_area_set_hide(const short area_id, const int state)
{
  UI_Area * const area = get_area(area_id);
  if (area->hide != state) {
    area->hide = state;
    update_visibility(area);
    update_active_area();
  }
}

static void area_draw(UI_Area *area, const short source_id)
{
  Window_ui_viewport(area->abs_pos, area->size, area->vis_pos);
  Connector_handle_execute(area->handle, OnDraw, source_id);
}

static void area_draw_siblings(short area_id)
{
  while (area_id) {
    UI_Area *area = get_area(area_id);
    if (is_visible(area)) {
      // draw children first
      area_draw_siblings(area->last_child);
      // draw self
      area_draw(area, area_id);
    }
    // get previous sibling
    area_id = area->prev_sibling;
  }
}

void UI_area_draw(const short area_id)
{
  if (area_id) {
    UI_Area *area = get_area(area_id);
    if (is_visible(area)) {
      // draw children first
      // start with most recent child added ie last child in list
      area_draw_siblings(area->last_child);
      // draw self
      area_draw(area, area_id);
    }
  }
}

void UI_area_root_draw(void)
{
  UI_area_draw(root_area);
}

uint8_t UI_area_modid(const short area_id)
{
  return get_area(area_id)->modid;
}

void UI_area_set_handled(const short area_id)
{
  get_area(area_id)->handled = 1;
}

int UI_area_handled(const short area_id)
{
  return get_area(area_id)->handled;
}

void UI_area_set_layout(const short area_id, const short layout)
{
  get_area(area_id)->layout = layout;
}

short UI_area_prev_sibling(const short area_id)
{
  return get_area(area_id)->prev_sibling;
}

short UI_area_next_sibling(const short area_id)
{
  return get_area(area_id)->next_sibling;
}

int UI_area_active_key(void)
{
  return active_key;
}

short UI_area_pointer_x(void)
{
  return pointer_x;
}

short UI_area_pointer_y(void)
{
  return pointer_y;
}

void UI_area_drag_start(void)
{
  drag_active = 1;
  old_y = pointer_y;
  old_x = pointer_x;
}

void UI_area_drag_end(void)
{
  drag_active = 0;
}

int UI_area_drag_delta_xy(void)
{
  return (pointer_x - old_x) + (old_y - pointer_y);
}

