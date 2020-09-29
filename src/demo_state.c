/*
* demo_state.c
*/

#include <stdlib.h>
#include <stdio.h>

#include "gles3.h"
#include "gear.h"
#include "window_manager.h"

typedef struct
{
// number of seconds to run the demo
   GLuint timeToRun;
   short tex;
   short gear1, gear2, gear3;
   GLfloat LightSourcePosition[4];
   GLboolean LightDirty;

// current angle of the gear
   GLfloat angle;
// the degrees per second the gear should rotate at
   GLfloat angleVel;
   int instances;
   GLboolean use_VBO;   

} DEMO_STATE_T;


static DEMO_STATE_T _state;
static DEMO_STATE_T *demo_state;

static void update_gear_VBO_use(void)
{
  if (demo_state->use_VBO) {
    Gear_all_use_BO();
    printf("using Buffer Objects for vertex/index data\n");
  }
  else {
    Gear_all_no_BO();
    printf("Not using Buffer Objects for vertex/index data\n");
  }
}

void DS_toggle_VBO(void)
{
  if (demo_state) {
    demo_state->use_VBO = demo_state->use_VBO ? GL_FALSE : GL_TRUE;
    update_gear_VBO_use();
  }
}

int DS_use_VBO(void)
{
  int result = 0;

  if (demo_state) {
    result = demo_state->use_VBO;
  }

  return result;
}

void DS_change_instances(const int val)
{
  demo_state->instances += val;
  if (demo_state->instances < 1) {
    demo_state->instances = 1;
  }
}

void DS_change_angleVel(const float val)
{
  demo_state->angleVel += val;
}

static void demo_state_delete(void)
{
  if (!demo_state) {
  // release memory used for gear and associated vertex arrays
    Gear_delete_all();
    demo_state->gear1 = 0;
    demo_state->gear2 = 0;
    demo_state->gear3 = 0;
    demo_state = 0;
  }
  printf("demo state has shut down\n");

}

static void demo_state_init(DEMO_STATE_T *state)
{
  //state->rate = 1.0f;
  state->angleVel = -30.0f;
  state->LightSourcePosition[0] = -8.0f;
  state->LightSourcePosition[1] = 5.0f;
  state->LightSourcePosition[2] = 15.0f;
  state->LightSourcePosition[3] = 1.0f;
  state->LightDirty = GL_TRUE;
  state->instances = 1;
  
  atexit(demo_state_delete);

}

static DEMO_STATE_T *get_demo_state(void)
{
  if (!demo_state) {
    demo_state = &_state;
    demo_state_init(demo_state);
  }

  return demo_state;
}

static void ds_build_gear1()
{
  const GLfloat red[4] = {0.8f, 0.2f, 0.2f, 1.0f};

  DEMO_STATE_T *state = get_demo_state();
  state->gear1 = Gear_create(1.0f, 4.0f, 1.25f, 20, 0.7f, red);
}

static void ds_build_gear2()
{
  const GLfloat green[4] = {0.2f, 0.8f, 0.2f, 1.0f};

  DEMO_STATE_T *state = get_demo_state();
  state->gear2 = Gear_create(0.5f, 2.0f, 1.50f, 10, 0.7f, green);
}

static void ds_build_gear3()
{
  const GLfloat blue[4] = {0.2f, 0.2f, 0.8f, 1.0f};

  DEMO_STATE_T *state = get_demo_state();
  state->gear3 = Gear_create(1.3f, 2.0f, 0.75f, 10, 0.7f, blue);
}

short DS_gear1(void)
{
  DEMO_STATE_T *state = get_demo_state();
  
  if (!state->gear1) {
    ds_build_gear1();
  }
  
  return state->gear1;
}

short DS_gear2(void)
{
  DEMO_STATE_T *state = get_demo_state();
  
  if (!state->gear2) {
    ds_build_gear2();
  }
  
  return state->gear2;
}

short DS_gear3(void)
{
  DEMO_STATE_T *state = get_demo_state();
  
  if (!state->gear3) {
    ds_build_gear3();
  }
  
  return state->gear3;
}

GLuint DS_timeToRun(void)
{
  return get_demo_state()->timeToRun;
}

short DS_tex(void)
{
  return get_demo_state()->tex;
}

GLfloat DS_angle(void)
{
  return get_demo_state()->angle;
}

float DS_angleVel(void)
{
  return get_demo_state()->angleVel;
}

void DS_update_timeToRun(const GLuint val)
{
  get_demo_state()->timeToRun = val;
}

void DS_update_angleVel(const GLfloat val)
{
  get_demo_state()->angleVel = val;
}

void DS_update_tex(const int id)
{
  get_demo_state()->tex = id;
}

GLuint DS_instances(void)
{
  return get_demo_state()->instances;
}

void DS_update_gear_rotation(void)
{
  DEMO_STATE_T *state = get_demo_state();
  /* advance gear rotation for next frame */
  state->angle += state->angleVel * WM_period_rate();
  if (state->angle > 360.0f) {
    state->angle -= 360.0f;
  }
  if (state->angle < 0.0f) {
    state->angle += 360.0f;
  }
}

void DS_light_move_y(const float val)
{
  DEMO_STATE_T *state = get_demo_state();
  state->LightSourcePosition[1] += val;
  state->LightDirty = GL_TRUE;
  printf("Light Y= %f\n", state->LightSourcePosition[1]);
}

void DS_light_move_x(const float val)
{
  DEMO_STATE_T *state = get_demo_state();
  state->LightSourcePosition[0] += val;
  state->LightDirty = GL_TRUE;
  printf("Light X= %f\n", state->LightSourcePosition[0]);
}

GLboolean DS_light_isDirty(void)
{
  return get_demo_state()->LightDirty;
}

void DS_light_clean(void)
{
  get_demo_state()->LightDirty = GL_FALSE;
}

GLfloat *DS_LightSourcePosition(void)
{
  return get_demo_state()->LightSourcePosition;
}

void DS_angleVel_pause(void)
{
  get_demo_state()->angleVel = 0.0f;
}
