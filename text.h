// text.h

#ifndef TEXT_H_
  #define TEXT_H_
    
  int text_create(void);
  void text_set_font(const int id, const int font);
  void text_add(const int id, const int x, const int y, const char *str);
  void text_set_start(const int id, const int index);
  int text_start(const int id);
  void text_draw(const int id);
  
#endif
