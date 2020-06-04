// ui_text.h

#ifndef _UI_TEXT_H_
  #define _UI_TEXT_H_

  short UI_text_create(void);
  short UI_text_text_id(const short id);
  int UI_text_area_handler(const short ui_text_id);
  void UI_text_add(const short id, const char *str);
#endif
