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
