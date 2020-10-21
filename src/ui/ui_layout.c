// ui_layout.c

#include <stdio.h>
#include <stdint.h>

#include "static_array.h"
#include "ui_area.h"

typedef struct {
  uint8_t active;
  
  uint8_t strategy;

  short spacing_vertical;
  short spacing_horizontal;

  short border_top;
  short border_bottom;
  short border_left;
  short border_right;

} UI_Layout;

#define UI_LAYOUT_MAX_COUNT 10

static UI_Layout layouts[UI_LAYOUT_MAX_COUNT];
static short next_deleted_layout;

static inline short find_deleted_layout(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_layout, layouts, UI_LAYOUT_MAX_COUNT, UI_Layout, "UI layout");
}

static UI_Layout *get_layout(short id)
{
  if ((id < 0) | (id >= UI_LAYOUT_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad UI Layout id, using default id: 0\n");
  }
    
  return layouts + id;
}

static void layout_init(UI_Layout *layout)
{
  layout->border_top = 10;
  layout->border_bottom = 10;
  layout->border_left = 10;
  layout->border_right = 10;

  layout->spacing_vertical = 2;
  layout->spacing_horizontal = 2;
}

short UI_Layout_create(void)
{
  const int id = find_deleted_layout();
  UI_Layout *const layout = get_layout(id);
  layout->active = 1;
  layout_init(layout);

  return id;
}

void UI_Layout_set_border(const short id, const short top, const short bottom, const short left, const short right)
{
  UI_Layout *const layout = get_layout(id);
  layout->border_top = top;
  layout->border_bottom = bottom;
  layout->border_left = left;
  layout->border_right = right;
}

void UI_Layout_set_spacing(const short id, const short vertical, const short horizontal)
{
  UI_Layout *const layout = get_layout(id);
  layout->spacing_vertical = vertical;
  layout->spacing_horizontal = horizontal;
}

void UI_Layout_update(const short id, short area_id)
{

  if (area_id) {
    short pos[2];
    short size[2];
    
    UI_Layout *const layout = get_layout(id);
    // get prev area layout and add vertical size + vertical spacing
    short sibling = UI_area_prev_sibling(area_id);
    // if prev area is null then use border top and border left to set layout
    if (sibling) {
      UI_area_layout_position(sibling, pos);
      UI_area_size(sibling, size);
      pos[1] += size[1] + layout->spacing_vertical;
    }
    else {
      pos[0] = layout->border_top;
      pos[1] = layout->border_left;
    }
    
    do {
      UI_area_set_layout_position(area_id, pos[0], pos[1]);
      area_id = UI_area_next_sibling(area_id);
      if (area_id) { 
        UI_area_size(area_id, size);
        pos[1] += size[1] + layout->spacing_vertical;
      }
      printf("doing layout...\n");
    } while (area_id);
    // if next area then update it
  }
}

