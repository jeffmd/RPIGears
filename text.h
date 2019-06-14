// text.h

#ifndef TEXT_H_
  #define TEXT_H_
  
  typedef struct Text Text; 
  
  Text *text_create(void);
  void text_set_font(Text *text, Font *font);
  void text_set_scale(Text *text, float scale);
  void text_add(Text *text, const int x, const int y, const char *str);
  void text_draw(Text *text);
#endif
