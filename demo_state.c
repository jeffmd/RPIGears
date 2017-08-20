/*
* demo_state.c
*/

#include <stdlib.h>
#include <string.h>

#include "GLES2/gl2.h"

#include "demo_state.h"
#include "gear.h"

typedef struct
{
// number of seconds to run the demo
   uint timeToRun;
   GLuint texId;

   int gear1, gear2, gear3;
   
// The location of the shader uniforms 
   GLuint ModelViewProjectionMatrix_location,
      ModelViewMatrix_location,
      NormalMatrix_location,
      LightSourcePosition_location,
      MaterialColor_location,
      DiffuseMap_location;
   
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


uint state_timeToRun(void)
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

void update_timeToRun(const uint val)
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
  state->avgfps = state->avgfps * 0.4f + fps * 0.6f;
  state->period_rate = 1.0f / state->avgfps;
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

GLuint state_ModelViewProjectionMatrix_location(void)
{
  return state->ModelViewProjectionMatrix_location;
}

GLuint state_ModelViewMatrix_location(void)
{
  return state->ModelViewMatrix_location;
}

GLuint state_NormalMatrix_location(void)
{
  return state->NormalMatrix_location;
}

GLuint state_LightSourcePosition_location(void)
{
  return state->LightSourcePosition_location;
}

GLuint state_MaterialColor_location(void)
{
  return state->MaterialColor_location;
}

GLuint state_DiffuseMap_location(void)
{
  return state->DiffuseMap_location;
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

void build_gears(const int useVBO)
{
  const GLfloat red[4] = {0.9, 0.3, 0.3, 1.0};
  const GLfloat green[4] = {0.3, 0.9, 0.3, 1.0};
  const GLfloat blue[4] = {0.3, 0.3, 0.9, 1.0};

  /* make the meshes for the gears */
  state->gear1 = gear(1.0, 4.0, 2.5, 20, 0.7, red);
  state->gear2 = gear(0.5, 2.0, 3.0, 10, 0.7, green);
  state->gear3 = gear(1.3, 2.0, 1.5, 10, 0.7, blue);
  
  // if VBO enabled then set them up for each gear
  if (useVBO) {
    make_gear_vbo(state->gear1);
    make_gear_vbo(state->gear2);
    make_gear_vbo(state->gear3);
  }
  else {
    set_gear_va_ptrs(state->gear1);   
    set_gear_va_ptrs(state->gear2);   
    set_gear_va_ptrs(state->gear3);   
  }

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

void update_uniform_location(const GLuint program)
{
   state->ModelViewProjectionMatrix_location = glGetUniformLocation(program, "ModelViewProjectionMatrix");
   state->ModelViewMatrix_location = glGetUniformLocation(program, "ModelViewMatrix");
   state->NormalMatrix_location = glGetUniformLocation(program, "NormalMatrix");
   state->LightSourcePosition_location = glGetUniformLocation(program, "LightSourcePosition");
   state->MaterialColor_location = glGetUniformLocation(program, "MaterialColor");
   state->DiffuseMap_location = glGetUniformLocation(program, "DiffuseMap");
}


void init_demo_state(void)
{
  memset( state, 0, sizeof( *state ) );

  state->rate = 1.0f;
  state->avgfps = 300.0f;
  state->period_rate = 1.0f / state->avgfps;
  state->angleVel = 70.0f;
  
  update_angleFrame();
}
