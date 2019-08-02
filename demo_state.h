/*
* demo_state.h
*/

#ifndef _demo_state_h_
  #define _demo_state_h_

// a procedure that takes a value representing the rate change to apply to an update

void demo_state_init(void);
void demo_state_build_gears(const int useVBO);
void demo_state_delete(void);
void demo_state_toggle_VBO(void);
GLuint state_timeToRun(void);
GPUTexture *state_tex(void);
gear_t *state_gear1(void);
gear_t *state_gear2(void);
gear_t *state_gear3(void);
GLfloat state_angle(void);
GLuint state_instances(void);
void demo_state_next_frame(void);
char *demo_state_has_fps(void);
  
void update_timeToRun(const GLuint val);
void update_angleVel(const GLfloat val);
void update_gear_rotation(void);
void light_move_y(const float val);
void light_move_x(const float val);
GLfloat *state_LightSourcePosition(void);
GLboolean light_isDirty(void);
void light_clean(void);
void update_tex(GPUTexture *tex);

#endif
