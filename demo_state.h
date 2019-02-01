/*
* demo_state.h
*/

#ifndef _demo_state_h_
  #define _demo_state_h_

#include "GLES2/gl2.h"
 
// a procedure that takes a value representing the rate change to apply to an update
typedef void (*UPDATE_KEY_DOWN)(const float);

GLuint state_timeToRun(void);
GLuint state_texId(void);
int state_gear1(void);
int state_gear2(void);
int state_gear3(void);
GLfloat state_angle(void);
  
void update_timeToRun(const GLuint val);
void update_angleVel(const GLfloat val);
void update_avgfps(const float fps);
void update_angleFrame(void);
void update_rate_frame(void);
void change_angleVel(const float val);
void update_avgfps(const float fps);
void update_angleFrame(void);
void update_rate_frame(void);
void change_angleVel(const float val);
void update_gear_rotation(void);
void move_rate_on(void);
void move_rate_off(void);
void inc_move_rate(void);
void init_demo_state(void);
void light_move_y(const float val);
void light_move_x(const float val);
GLfloat *state_LightSourcePosition(void);
GLboolean light_isDirty(void);
void light_clean(void);
void update_texId(const GLuint texId);
GLuint state_texId(void);
void build_gears(const int useVBO);
void set_key_down_update(UPDATE_KEY_DOWN fn, float val);
void do_key_down_update(void);

#endif
