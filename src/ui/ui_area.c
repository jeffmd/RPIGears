// ui_area.c - user interface area

#include <stdio.h>
#include <stdint.h>

#include "window.h"
#include "static_array.h"
#include "action_table.h"
#include "handler.h"

typedef struct {
  uint8_t active;
  // area id links
  short parent;
  short child;
  short sibling;

  short pos[2];
  short size[2];

  // handler link
  short handler;

  // flags
  unsigned hide:1;
  unsigned visible:1;
  unsigned selectable:1;

} UI_Area;

enum Events {
  OnAttach,
  OnEnter,
  OnLeave,
  OnMove,
  OnResize,
  OnDraw,
  EventsMax
};

#define UI_AREA_MAX_COUNT 100

static UI_Area areas[UI_AREA_MAX_COUNT];
static short next_deleted_area;

static short root_area_id = 0;
static short active_area_id = 0;

static short get_active_area_id(void)
{
  if (active_area_id)
    return active_area_id;
  else
    return root_area_id;
}

static inline short find_deleted_area_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_area, areas, UI_AREA_MAX_COUNT, UI_Area, "UI area");
}

static UI_Area *get_area(int id)
{
  if ((id < 0) | (id >= UI_AREA_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI Area id, using default id: 0\n");
  }
    
  return areas + id;
}

static void area_init(UI_Area *area)
{
  area->sibling = 0;
  area->parent = 0;
  area->child = 0;
  area->handler = 0;
}

static void area_root_pos(UI_Area *area, int pos[2])
{
  short area_id = area->parent;

  pos[0] = area->pos[0];
  pos[1] = area->pos[1];

  while (area_id) {
    area = get_area(area_id);
    area_id = area->parent;
    pos[0] += area->pos[0];
    pos[1] += area->pos[1];
  }
}

static int area_inside(UI_Area *area, const int x, const int y)
{
  int is_inside = 0;
  int pos[2];
  area_root_pos(area, pos);

  if ( (x >= pos[0]) && ( x <= (pos[0] + area->size[0]))) {
    if ((y >= pos[1]) && ( y <= (pos[1] + area->size[1]))) {
      is_inside = 1;
    }
  }
  return is_inside;
}

static int child_inside_parent(UI_Area *area)
{
  int is_inside = 0;
  int pos[2];
  area_root_pos(area, pos);

  if (area->parent) {
    UI_Area *parent = get_area(area->parent);

    if (
      area_inside(parent, pos[0], pos[1]) ||
      area_inside(parent, pos[0] + area->size[0], pos[1]) ||
      area_inside(parent, pos[0] + area->size[0], pos[1] + area->size[1]) ||
      area_inside(parent, pos[0], pos[1] + area->size[1])

      ) {
        is_inside = 1;
    }
  } else {
    is_inside = 1;
  }
  
  return is_inside;
} 

static void update_visibility(UI_Area *area)
{
  if (!area->hide) {
    area->visible = child_inside_parent(area);
  }
  else {
    area->visible = !area->hide;
  }
}

short UI_area_create(void)
{
  const int id = find_deleted_area_id();
  UI_Area *const area = get_area(id);
  area->active = 1;
  area_init(area);

  return id;
}

void UI_area_remove_parent(const short area_id)
{
  UI_Area * const area = get_area(area_id);
  if (area->parent) {
    UI_Area *parent = get_area(area->parent);

    if (parent->child == area_id) {
      parent->child = area->sibling;
    }
    else {
      int next = parent->child;
      while (next) {
        UI_Area *sibling = get_area(next);
        next = sibling->sibling;
        if (next == area_id) {
          sibling->sibling = area->sibling;
          next = 0;
        }
      }
    }
  }

  area->sibling = 0;
  area->parent = 0;
}

void UI_area_add(const short parent_id, const short child_id)
{
  UI_Area * child = get_area(child_id);
  if (child->parent != parent_id) {
    UI_area_remove_parent(child_id);

    UI_Area * parent = get_area(parent_id);
    child->sibling = parent->child;
    child->parent = parent_id;
    parent->child = child_id;
  }

  update_visibility(child);
}

void UI_area_set_active(const short area_id)
{
  if (active_area_id != area_id) {
    UI_Area *area = get_area(active_area_id);
    Handler_execute(area->handler, OnLeave, active_area_id);
    active_area_id = area_id;
    area = get_area(area_id);
    Handler_execute(area->handler, OnEnter, area_id);
    //printf("inside area: %i\n", area_id);
  }
}

int UI_area_is_active(const short area_id)
{
  return (area_id == active_area_id);
}

void UI_area_set_root(const short area_id)
{
  root_area_id = area_id;
}

void UI_area_set_position(const short area_id, const int x, const int y)
{
  UI_Area * const area = get_area(area_id);
  area->pos[0] = x;
  area->pos[1] = y;
  update_visibility(area);
  Handler_execute(area->handler, OnMove, area_id);
}

void UI_area_set_size(const short area_id, const int width, const int height)
{
  UI_Area * const area = get_area(area_id);
  area->size[0] = width;
  area->size[1] = height;
  update_visibility(area);
  Handler_execute(area->handler, OnResize, area_id);
}

void UI_area_size(const short area_id, int size[2])
{
  UI_Area * const area = get_area(area_id);
  size[0] = area->size[0];
  size[1] = area->size[1];
}

static short area_find(short area_id, const int check_sibling, const int x, const int y)
{
  short newhit = 0;

  while (area_id) {
    UI_Area *area = get_area(area_id);

    if (area->visible && area_inside(area, x, y)) {
      // check children
      newhit = area_find(area->child, 1, x, y);
      if (!newhit) {
        newhit = area_id;
      }
      area_id = 0;
    }
    else {
      area_id = check_sibling ? area->sibling : area->parent;
    }
  }

  return newhit;
}

void UI_area_select_active(const int x, const int y)
{
  UI_area_set_active(area_find(get_active_area_id(), 0, x, y));
}

short UI_area_create_action_table(void)
{
  const short table_id = Action_table_create();
  Action_table_allocate_slots(table_id, EventsMax);

  return table_id;
}

void UI_area_action_set_enter(const short table_id, ActionFn action)
{
  Action_table_set_action(table_id, OnEnter, action);
}

void UI_area_action_set_leave(const short table_id, ActionFn action)
{
  Action_table_set_action(table_id, OnLeave, action);
}

void UI_area_action_set_draw(const short table_id, ActionFn action)
{
  Action_table_set_action(table_id, OnDraw, action);
}

void UI_area_action_set_resize(const short table_id, ActionFn action)
{
  Action_table_set_action(table_id, OnResize, action);
}

void UI_area_action_set_move(const short table_id, ActionFn action)
{
  Action_table_set_action(table_id, OnMove, action);
}

void UI_area_action_set_attach(const short table_id, ActionFn action)
{
  Action_table_set_action(table_id, OnAttach, action);
}

void UI_area_set_handler(const short area_id, const short handler_id)
{
  UI_Area * const area = get_area(area_id);
  area->handler = handler_id;
  Handler_execute(area->handler, OnAttach, area_id);
}

void UI_area_set_hide(const short area_id, const int state)
{
  UI_Area * const area = get_area(area_id);
  area->hide = state;
  update_visibility(area);
}

static void area_draw(UI_Area *area, const short source_id)
{
  int pos[2];

  area_root_pos(area, pos);
  window_ui_viewport(pos[0], pos[1], area->size[0], area->size[1]);
  Handler_execute(area->handler, OnDraw, source_id);
}

static void area_draw_siblings(short area_id)
{
  while (area_id) {
    UI_Area *area = get_area(area_id);
    // draw children first
    if (area->visible) {
      area_draw_siblings(area->child);
      // draw self
      area_draw(area, area_id);
    }
    // get sibling
    area_id = area->sibling;
  }
}

void UI_area_draw(const short area_id)
{
  if (area_id) {
    UI_Area *area = get_area(area_id);
    if (area->visible) {
      // draw children first
      area_draw_siblings(area->child);
      // draw self
      area_draw(area, area_id);
    }
  }
}

void UI_area_root_draw(void)
{
  UI_area_draw(root_area_id);
}
