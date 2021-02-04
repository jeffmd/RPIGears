// ui_text.h

#ifndef _UI_TEXT_H_
  #define _UI_TEXT_H_

  Handle_t UI_text_create(const char *str);
  ID_t UI_text_text_id(const ID_t id);
  void UI_text_add(const ID_t id, const char *str);
#endif
