/*
* demo_state.h
*/

#ifndef _demo_state_h_
  #define _demo_state_h_

#include "GLES/gl.h"
 
// a procedure that takes a value representing the rate change to apply to an update
typedef void (*UPDATE_KEY_DOWN)(const float);

extern uint state_timeToRun(void);
extern GLuint state_texId(void);
extern int state_gear1(void);
extern int state_gear2(void);
extern int state_gear3(void);
extern GLfloat state_angle(void);
extern GLuint state_CameraProjectionMatrix_location(void);
extern GLuint state_ModelViewMatrix_location(void);
extern GLuint state_NormalMatrix_location(void);
extern GLuint state_LightSourcePosition_location(void);
extern GLuint state_MaterialColor_location(void);
extern GLuint state_DiffuseMap_location(void);
  
extern void update_timeToRun(const uint val);
extern void update_angleVel(const GLfloat val);
extern void update_avgfps(const float fps);
extern void update_angleFrame(void);
extern void update_rate_frame(void);
extern void change_angleVel(const float val);
extern void update_avgfps(const float fps);
extern void update_angleFrame(void);
extern void update_rate_frame(void);
extern void change_angleVel(const float val);
extern void update_gear_rotation(void);
extern void move_rate_on(void);
extern void move_rate_off(void);
extern void inc_move_rate(void);
extern void init_demo_state(void);
extern void update_texId(const GLuint texId);
extern GLuint state_texId(void);
extern void build_gears(const int useVBO);
extern void set_key_down_update(UPDATE_KEY_DOWN fn, float val);
extern void update_uniform_location(const GLuint program);
extern void do_key_down_update(void);

#endif
