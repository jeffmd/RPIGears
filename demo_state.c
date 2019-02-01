/*
* demo_state.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "demo_state.h"
#include "gear.h"

typedef struct
{
// number of seconds to run the demo
   GLuint timeToRun;
   GLuint texId;
   GLfloat LightSourcePosition[4];
   GLboolean LightDirty;
   int gear1, gear2, gear3;

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

   // keyboard data
   UPDATE_KEY_DOWN key_down_update; // points to a function that gets executed each time a key goes down or repeats
   float rate; // the rate at which a change occurs
   int rate_enabled; // if enabled the change_rate will increase each frame
   float rate_direction; // direction and scale for rate change
   float rate_frame; // how much the rate changes each frame

} DEMO_STATE_T;


static DEMO_STATE_T _state;
static DEMO_STATE_T * const state = &_state;


GLuint state_timeToRun(void)
{
  return state->timeToRun;
}

GLuint state_texId(void)
{
  return state->texId;
}

int state_gear1(void)
{
  return state->gear1;
}

int state_gear2(void)
{
  return state->gear2;
}

int state_gear3(void)
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

void update_texId(const GLuint texId)
{
  state->texId = texId;
}

GLuint state_textId(void)
{
  return state->texId;
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

  /* make the meshes for the gears */
  state->gear1 = gear(1.0, 4.0, 2.5, 20, 0.7, red, useVBO);
  state->gear2 = gear(0.5, 2.0, 3.0, 10, 0.7, green, useVBO);
  state->gear3 = gear(1.3, 2.0, 1.5, 10, 0.7, blue, useVBO);

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
  update_angleFrame();
}
