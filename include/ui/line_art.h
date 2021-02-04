// line_art.h

#ifndef _LINE_ART_H_
  #define _LINE_ART_H_

  ID_t Line_Art_create(void);
  void Line_Art_end(const ID_t id);
  void Line_Art_add(const float x, const float y);
  void Line_Art_draw(const ID_t batch_part);
  void Line_Art_set_scale_offset(float scale_x, float scale_y, float offset_x, float offset_y);
  
#endif