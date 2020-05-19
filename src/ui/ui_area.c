// ui_area.c - user interface area

#include <stdio.h>
#include <stdint.h>

#include "window.h"
#include "static_array.h"
#include "action_table.h"
#include "handler.h"
#include "key_input.h"

typedef struct {
  uint8_t active;
  // area id links
  short parent;
  short child;
  short sibling;

  short rel_pos[2];
  short size[2];
  short abs_pos[2];
  short vis_pos[4];

  uint8_t modid;
  uint8_t parent_modid;

  // handler link
  short handler;

  // flags
  unsigned hide:1;
  unsigned visible:1;
  //unsigned selectable:1;
  unsigned handled:1;

} UI_Area;

enum Events {
  OnAttach,
  OnEnter,
  OnLeave,
  OnMove,
  OnResize,
  OnDraw,
  OnKeyChange,
  EventsMax
};

#define UI_AREA_MAX_COUNT 100

static UI_Area areas[UI_AREA_MAX_COUNT];
static short next_deleted_area;

static short root_area_id = 0;
static short active_area_id = 0;
static int active_key;

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
  area->modid++;
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

    area->abs_pos[0] = pos[0];
    area->abs_pos[1] = pos[1];
    // clip pos to parent vis_pos
    clip_pos(parent_area->vis_pos, pos);
    clip_pos(parent_area->vis_pos, pos + 2);

    area->parent_modid = parent_area->modid;
  }
  
  area->modid++;
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
  }
}

static int is_visible(UI_Area *area)
{
  if (area->parent) {
    if (area->parent_modid != get_area(area->parent)->modid) {
      update_visibility(area);
      printf("udate visibility\n");
    }
  }

  return area->visible;
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
  area->rel_pos[0] = x;
  area->rel_pos[1] = y;
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

    if (is_visible(area) && area_inside(area->vis_pos, x, y)) {
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

void UI_area_key_change(const int key)
{
  int handled = 0;
  short area_id = get_active_area_id();
  active_key = key;

  while (area_id) {
    UI_Area *area = get_area(area_id);
    area->handled = 0;
    Handler_execute(area->handler, OnKeyChange, area_id);
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

void UI_area_action_set_keyChange(const short table_id, ActionFn action)
{
  Action_table_set_action(table_id, OnKeyChange, action);
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
  window_ui_viewport(area->abs_pos, area->size, area->vis_pos);
  Handler_execute(area->handler, OnDraw, source_id);
}

static void area_draw_siblings(short area_id)
{
  while (area_id) {
    UI_Area *area = get_area(area_id);
    if (is_visible(area)) {
      // draw children first
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
    if (is_visible(area)) {
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

uint8_t UI_area_modid(const short area_id)
{
  return get_area(area_id)->modid;
}

void UI_area_set_handled(const short area_id)
{
  get_area(area_id)->handled = 1;
}

int UI_area_active_key(void)
{
  return active_key;
}