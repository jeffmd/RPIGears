// ui_edit_text.h

#ifndef _UI_EDIT_TEXT_H_
  #define _UI_EDIT_TEXT_H_

  void UI_edit_text_start(char *str, const short length);
  void UI_edit_text_stop(void);
  int UI_edit_text_cursor_moved(void);
  int UI_edit_text_changed(void);
  short UI_edit_text_cursor_index(void);
  int UI_edit_text_set_cursor_index(short index);
  int UI_edit_text_cursor_blink(void);
  void UI_edit_text_set_changed(void);
  void UI_edit_text_number_key_change(const ID_t area_id, const ID_t ui_id);
  void UI_edit_text_key_change(const ID_t area_id, const ID_t ui_id);
  int UI_edit_text_insert_mode(void);

#endif