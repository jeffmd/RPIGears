/*
* demo_state.h
*/

#ifndef _demo_state_h_
  #define _demo_state_h_

  GLuint DS_timeToRun(void);
  ID_t DS_tex(void);
  ID_t DS_gear1(void);
  ID_t DS_gear2(void);
  ID_t DS_gear3(void);
  GLfloat DS_angle(void);
  GLuint DS_instances(void);
  float DS_angleVel(void);
  void DS_toggle_VBO(void);
  int DS_use_VBO(void);
  void DS_set_instances(const int val);
  void DS_change_instances(const int val);
  void DS_set_angleVel(const float val);
  void DS_change_angleVel(const float val);
  void DS_state_next_frame(void);
  char *DS_has_fps(void);
  
  void DS_set_timeToRun(const GLuint val);
  void DS_update_gear_rotation(void);
  void DS_light_move_y(const float val);
  void DS_light_move_x(const float val);
  GLfloat *DS_LightSourcePosition(void);
  GLboolean DS_light_isDirty(void);
  void DS_light_clean(void);
  void DS_set_tex(const int id);
  void DS_angleVel_pause(void);

#endif
