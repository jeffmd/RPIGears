// ui_edit_text.h

#ifndef _UI_EDIT_TEXT_H_
  #define _UI_EDIT_TEXT_H_

  void UI_edit_text_start(char *str, const short length);
  void UI_edit_text_stop(void);
  short UI_edit_text_key_map(void);
  int UI_edit_text_cursor_moved(void);
  int UI_edit_text_changed(void);
  short UI_edit_text_cursor_index(void);
  int UI_edit_text_set_cursor_index(short index);
  int UI_edit_text_cursor_blink(void);
  void UI_edit_text_set_changed(void);

#endif