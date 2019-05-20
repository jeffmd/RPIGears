/*
* demo_state.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "gear.h"
#include "gpu_texture.h"
#include "demo_state.h"

typedef struct
{
// number of seconds to run the demo
   GLuint timeToRun;
   GPUTexture *tex;
   GLfloat LightSourcePosition[4];
   GLboolean LightDirty;
   gear_t *gear1, *gear2, *gear3;

// current angle of the gear
   GLfloat angle;
// the degrees that the angle should change each frame
   GLfloat angleFrame;
// the degrees per second the gear should rotate at
   GLfloat angleVel;
// Average Frames Per Second
   float avgfps;
   // the average time between each frame update = 1/avgfps
   float period_rate;
   int instances;
   GLboolean use_VBO;
   
   // keyboard data
   UPDATE_KEY_DOWN key_down_update; // points to a function that gets executed each time a key goes down or repeats
   float rate; // the rate at which a change occurs
   int rate_enabled; // if enabled the change_rate will increase each frame
   float rate_direction; // direction and scale for rate change
   float rate_frame; // how much the rate changes each frame

} DEMO_STATE_T;


static DEMO_STATE_T _state;
static DEMO_STATE_T * const state = &_state;

static void update_gear_VBO_use(void)
{
  if (state->use_VBO) {
    gear_use_BO(state->gear1);
    gear_use_BO(state->gear2);
    gear_use_BO(state->gear3);
    printf("using Buffer Objects for vertex/index data\n");
  }
  else {
    gear_no_BO(state->gear1);
    gear_no_BO(state->gear2);
    gear_no_BO(state->gear3);
    printf("Not using Buffer Objects for vertex/index data\n");
  }
}

void state_toggle_VBO(void)
{
  state->use_VBO = state->use_VBO ? GL_FALSE : GL_TRUE;
  update_gear_VBO_use();
}

GLuint state_timeToRun(void)
{
  return state->timeToRun;
}

GPUTexture *state_tex(void)
{
  return state->tex;
}

gear_t *state_gear1(void)
{
  return state->gear1;
}

gear_t *state_gear2(void)
{
  return state->gear2;
}

gear_t *state_gear3(void)
{
  return state->gear3;
}

GLfloat state_angle(void)
{
  return state->angle;
}

void update_timeToRun(const GLuint val)
{
  state->timeToRun = val;
}

void update_angleVel(const GLfloat val)
{
  state->angleVel = val;
}

void change_angleVel(const float val)
{
  state->angleVel += val;
}

void update_avgfps(const float fps)
{
  //state->avgfps = state->avgfps * 0.4f + fps * 0.6f;
  state->period_rate = 1.0f / fps;
}

void update_angleFrame(void)
{
	state->angleFrame = state->angleVel * state->period_rate;
}

void update_rate_frame(void)
{
  state->rate_frame = state->rate * state->period_rate;
}

void update_tex(GPUTexture *tex)
{
  state->tex = tex;
}

GLuint state_instances(void)
{
  return state->instances;
}

void inc_instances(void)
{
  state->instances++;
}

void dec_instances(void)
{
  state->instances--;
  if (state->instances < 1) 
    state->instances = 1;	
}

void update_gear_rotation(void)
{
    /* advance gear rotation for next frame */
    state->angle += state->angleFrame;
    if (state->angle > 360.0)
      state->angle -= 360.0;
}

void move_rate_on(void)
{
  state->rate_enabled = 1;
}

void move_rate_off(void)
{
  state->rate_enabled = 0;
  state->rate = 1.0f;
  state->key_down_update = 0;
}

void inc_move_rate(void)
{
  // increase movement speed if not at max
  //if (state->move_rate < 40.0f)
  if (state->rate_enabled) state->rate += 10 * state->period_rate;
}

void light_move_y(const float val)
{
  state->LightSourcePosition[1] += val;
  state->LightDirty = GL_TRUE;
  printf("Light Y= %f\n", state->LightSourcePosition[1]);
}

void light_move_x(const float val)
{
  state->LightSourcePosition[0] += val;
  state->LightDirty = GL_TRUE;
  printf("Light X= %f\n", state->LightSourcePosition[0]);
}

GLboolean light_isDirty(void)
{
  return state->LightDirty;
}

void light_clean(void)
{
  state->LightDirty = GL_FALSE;
}

GLfloat *state_LightSourcePosition(void)
{
  return state->LightSourcePosition;
}

void build_gears(const int useVBO)
{
  const GLfloat red[4] = {0.8, 0.2, 0.2, 1.0};
  const GLfloat green[4] = {0.2, 0.8, 0.2, 1.0};
  const GLfloat blue[4] = {0.2, 0.2, 0.8, 1.0};

  state->use_VBO = useVBO;
  /* make the meshes for the gears */
  state->gear1 = gear(1.0, 4.0, 1.25, 20, 0.7, red);
  state->gear2 = gear(0.5, 2.0, 1.50, 10, 0.7, green);
  state->gear3 = gear(1.3, 2.0, 0.75, 10, 0.7, blue);

  update_gear_VBO_use();
}

void set_key_down_update(UPDATE_KEY_DOWN fn, float val)
{
  state->key_down_update = fn;
  state->rate_direction = val;
}

void do_key_down_update(void)
{
  if (state->key_down_update) {
    state->key_down_update(state->rate_direction * state->rate_frame);
  }
}

void init_demo_state(void)
{
  memset( state, 0, sizeof( *state ) );

  state->rate = 1.0f;
  state->avgfps = 300.0f;
  state->period_rate = 1.0f / state->avgfps;
  state->angleVel = -30.0f;
  state->LightSourcePosition[0] = -8.0;
  state->LightSourcePosition[1] = 5.0;
  state->LightSourcePosition[2] = 15.0;
  state->LightSourcePosition[3] = 1.0;
  state->LightDirty = GL_TRUE;
  state->instances = 1;
  update_angleFrame();
}
