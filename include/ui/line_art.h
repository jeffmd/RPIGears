// line_art.h

#ifndef _LINE_ART_H_
  #define _LINE_ART_H_

  short Line_Art_create(void);
  void Line_Art_end(const short id);
  void Line_Art_add(const float x, const float y);
  void Line_Art_draw(const short batch_part);
  void Line_Art_set_offset(float x, float y);
  
#endif