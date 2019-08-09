// text.h

#ifndef TEXT_H_
  #define TEXT_H_
    
  int text_create(void);
  void text_set_font(int id, int font);
  void text_add(int id, int x, int y, const char *str);
  void text_set_start(int id, const int index);
  int text_start(int id);
  void text_draw(int id);
  
#endif
