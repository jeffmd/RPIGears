// ui_layout.c

#ifndef _UI_LAYOUT_H_
  #define _UI_LAYOUT_H_

  short UI_Layout_create(void);
  void UI_Layout_set_border(const short id, const short top, const short bottom, const short left, const short right);
  void UI_Layout_set_spacing(const short id, const short vertical, const short horizontal);

#endif