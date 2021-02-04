// ui_layout.c

#ifndef _UI_LAYOUT_H_
  #define _UI_LAYOUT_H_

  ID_t UI_Layout_create(void);
  void UI_Layout_set_border(const ID_t id, const short top, const short bottom, const short left, const short right);
  void UI_Layout_set_spacing(const ID_t id, const short vertical, const short horizontal);
  void UI_Layout_update(const ID_t id, ID_t area_id);

#endif