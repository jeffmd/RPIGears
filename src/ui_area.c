// ui_area.c - user interface area

#include <stdio.h>
#include <stdint.h>

#include "static_array.h"

typedef struct UI_Area {
  uint8_t active;
  // area id links
  short parent;
  short child;
  short sibling;

  short pos[2];
  short size[2];

  // action link
  short action;

} UI_Area;

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

static inline int find_deleted_area_id(void)
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
}

int UI_area_create(void)
{
  const int id = find_deleted_area_id();
  UI_Area *const area = get_area(id);
  area->active = 1;
  area_init(area);

  return id;
}

void UI_area_remove_parent(const int area_id)
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

void UI_area_add(const int parent_id, const int child_id)
{
  UI_Area * child = get_area(child_id);
  if (child->parent != parent_id) {
    UI_area_remove_parent(child_id);

    UI_Area * parent = get_area(parent_id);
    short sibling_id = parent->child;
    parent->child = child_id;
    child->parent = parent_id;
    child->sibling = sibling_id;
  }
}

void UI_area_set_active(const int area_id)
{
  active_area_id = area_id;
}

int UI_area_is_active(const int area_id)
{
  return (area_id == active_area_id);
}

void UI_area_set_root(const int area_id)
{
  root_area_id = area_id;
}

void UI_area_set_position(const int area_id, const int x, const int y)
{
  UI_Area * const area = get_area(area_id);
  area->pos[0] = x;
  area->pos[1] = y;
}

void UI_area_set_size(const int area_id, const int width, const int height)
{
  UI_Area * const area = get_area(area_id);
  area->size[0] = width;
  area->size[1] = height;
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

static int area_inside(UI_Area* area, const int x, const int y)
{
  int is_inside = 0;
  int pos[2];
  area_root_pos(area, pos);

  if ( (x >= pos[0]) && ( x <= (pos[0] + area->size[0]))) {
    if ((y >= pos[1]) && ( y <= (pos[1] + area->size[1]))) {
      printf("inside x: %i, y: %i \n", x, y);
      is_inside = 1;
    }
  }
  return is_inside;
}

static short area_find(int area_id, const int check_sibling, const int x, const int y)
{
  short newhit = 0;

  while (area_id) {
    UI_Area *area = get_area(area_id);

    if (area_inside(area, x, y)) {
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
