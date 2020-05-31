/*
* demo_state.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gles3.h"
#include "gear.h"
#include "gpu_texture.h"
#include "key_input.h"
#include "window_manager.h"

typedef struct
{
// number of seconds to run the demo
   GLuint timeToRun;
   short tex;
   GLfloat LightSourcePosition[4];
   GLboolean LightDirty;
   short gear1, gear2, gear3;

// current angle of the gear
   GLfloat angle;
// the degrees per second the gear should rotate at
   GLfloat angleVel;
   int instances;
   GLboolean use_VBO;   

} DEMO_STATE_T;


static DEMO_STATE_T _state;
static DEMO_STATE_T * const state = &_state;


static void update_gear_VBO_use(void)
{
  if (state->use_VBO) {
    Gear_use_BO(state->gear1);
    Gear_use_BO(state->gear2);
    Gear_use_BO(state->gear3);
    printf("using Buffer Objects for vertex/index data\n");
  }
  else {
    Gear_no_BO(state->gear1);
    Gear_no_BO(state->gear2);
    Gear_no_BO(state->gear3);
    printf("Not using Buffer Objects for vertex/index data\n");
  }
}

void demo_state_toggle_VBO(void)
{
  state->use_VBO = state->use_VBO ? GL_FALSE : GL_TRUE;
  update_gear_VBO_use();
}

static void demo_state_toggle_VBO_key(const short souce_id, const short destination_id)
{
  demo_state_toggle_VBO();
}

GLuint state_timeToRun(void)
{
  return state->timeToRun;
}

int state_tex(void)
{
  return state->tex;
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

static void change_angleVel(const float val)
{
  state->angleVel += val;
}

void update_tex(const int id)
{
  state->tex = id;
}

GLuint state_instances(void)
{
  return state->instances;
}

static void inc_instances(const short souce_id, const short destination_id)
{
  state->instances++;
}

static void dec_instances(const short souce_id, const short destination_id)
{
  state->instances--;
  if (state->instances < 1) 
    state->instances = 1;	
}

void update_gear_rotation(void)
{
  /* advance gear rotation for next frame */
  state->angle += state->angleVel * WM_period_rate();
  if (state->angle > 360.0)
    state->angle -= 360.0;
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

void demo_state_build_gears(const int useVBO)
{
  const GLfloat red[4] = {0.8, 0.2, 0.2, 1.0};
  const GLfloat green[4] = {0.2, 0.8, 0.2, 1.0};
  const GLfloat blue[4] = {0.2, 0.2, 0.8, 1.0};

  state->use_VBO = useVBO;
  /* make the meshes for the gears */
  state->gear1 = Gear_create(1.0, 4.0, 1.25, 20, 0.7, red);
  state->gear2 = Gear_create(0.5, 2.0, 1.50, 10, 0.7, green);
  state->gear3 = Gear_create(1.3, 2.0, 0.75, 10, 0.7, blue);

  update_gear_VBO_use();
}

static void demo_state_delete(void)
{
  // release memory used for gear and associated vertex arrays
  Gear_delete(state->gear1);
  Gear_delete(state->gear2);
  Gear_delete(state->gear3);
  
  printf("demo state has shut down\n");

}

static void key_angleVel_down(const short souce_id, const short destination_id)
{
  Key_input_set_update(change_angleVel, -10.0f);
}

static void key_angleVel_up(const short souce_id, const short destination_id)
{
  Key_input_set_update(change_angleVel, 10.0f);
}

static void key_angleVel_pause(const short souce_id, const short destination_id)
{
  update_angleVel(0.0);
}

void demo_state_init(void)
{
  //state->rate = 1.0f;
  state->angleVel = -30.0f;
  state->LightSourcePosition[0] = -8.0;
  state->LightSourcePosition[1] = 5.0;
  state->LightSourcePosition[2] = 15.0;
  state->LightSourcePosition[3] = 1.0;
  state->LightDirty = GL_TRUE;
  state->instances = 1;
  
  Key_input_set_update(0, 0.0f);
  Key_add_action('b', demo_state_toggle_VBO_key, "toggle use of Buffer Objects for gear vertex data");
  Key_add_action(SHIFT_KEY('I'), inc_instances, "add another draw instance of the gears");
  Key_add_action(SHIFT_KEY('O'), dec_instances, "remove an instance of the gears");
  Key_add_action(SHIFT_KEY('<'), key_angleVel_down, "decrease gear spin rate");
  Key_add_action(SHIFT_KEY('>'), key_angleVel_up, "increase gear spin rate");
  Key_add_action('p', key_angleVel_pause, "stop gear spin");
  
  atexit(demo_state_delete);

}
