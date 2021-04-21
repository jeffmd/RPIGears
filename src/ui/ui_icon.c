// ui_icon.h

#include <stdio.h>

#include "id_plug.h"
#include "line_art.h"

static ID_t box_batch_part;
static ID_t checked_box_batch_part;

static void add_box(const float x, const float y)
{
    const float x2 = x + 1;
    const float y2 = y + 1;

    Line_Art_start(x, y);
    Line_Art_add(x2, y);
    Line_Art_add(x2, y2);
    Line_Art_add(x, y2);
    Line_Art_add(x, y);

}

static ID_t get_box_batch_part(void)
{
  if(!box_batch_part) {
    box_batch_part = Line_Art_create();
    add_box(0, 0);
    Line_Art_end(box_batch_part);
  }

  return box_batch_part;
}

static ID_t get_checked_box_batch_part(void)
{
  if(!checked_box_batch_part) {
    checked_box_batch_part = Line_Art_create();
    // box
    add_box(0, 0);
    // check mark
    Line_Art_start(0.25f, 0.7f);
    Line_Art_add(0.5f, 0.333f);
    Line_Art_add(1.3, 0.9);
    Line_Art_end(checked_box_batch_part);
  }

  return checked_box_batch_part;
}

void UI_icon_draw_box(float scale_x, float scale_y, float offset_x, float offset_y)
{
  Line_Art_set_scale_offset(scale_x, scale_y, offset_x, offset_y);
  Line_Art_draw(get_box_batch_part());
}

void UI_icon_draw_checked_box(float scale_x, float scale_y, float offset_x, float offset_y)
{
  Line_Art_set_scale_offset(scale_x, scale_y, offset_x, offset_y);
  Line_Art_draw(get_checked_box_batch_part());
}