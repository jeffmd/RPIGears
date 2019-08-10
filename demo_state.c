/*
* demo_state.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "gles3.h"
#include "gear.h"
#include "gpu_texture.h"
#include "tasks.h"
#include "key_input.h"

typedef struct
{
// number of seconds to run the demo
   GLuint timeToRun;
   int tex;
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
   int instances;
   GLboolean use_VBO;   

} DEMO_STATE_T;


static DEMO_STATE_T _state;
static DEMO_STATE_T * const state = &_state;

static int frames; // number of frames drawn since the last frame/sec calculation
static int lastFrames;
static char fps_str[12];
static char *fps_strptr;

static int AngleFrame_task;
static int FPS_task;


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

void demo_state_toggle_VBO(void)
{
  state->use_VBO = state->use_VBO ? GL_FALSE : GL_TRUE;
  update_gear_VBO_use();
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

static void update_angleFrame(void)
{
	state->angleFrame = state->angleVel * state->period_rate;
}


void update_avgfps(const float fps)
{
  //state->avgfps = state->avgfps * 0.6f + fps * 0.4f;
  //printf("fps: %f\n", fps);
  if ( fabsf(state->avgfps - fps) > 0.1f ) {
    sprintf(fps_str, "%3.1f  ", fps);
    fps_strptr = fps_str;
    state->period_rate = 1.0f / fps;
    state->avgfps = fps;
    update_angleFrame();
    key_input_set_rate_frame(state->period_rate);
  }
}

void update_tex(const int id)
{
  state->tex = id;
}

GLuint state_instances(void)
{
  return state->instances;
}

static void inc_instances(void)
{
  state->instances++;
}

static void dec_instances(void)
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
  state->gear1 = gear(1.0, 4.0, 1.25, 20, 0.7, red);
  state->gear2 = gear(0.5, 2.0, 1.50, 10, 0.7, green);
  state->gear3 = gear(1.3, 2.0, 0.75, 10, 0.7, blue);

  update_gear_VBO_use();
}

static void demo_state_delete(void)
{
  // release memory used for gear and associated vertex arrays
  gear_delete(state->gear1);
  gear_delete(state->gear2);
  gear_delete(state->gear3);
  
  printf("demo state has shut down\n");

}

static void do_AngleFrame_task(void)
{
  
  const float dt = task_elapsed(AngleFrame_task) / 1000.0f;
  if (dt > 0.0f) {
	  
    update_avgfps((float)(frames - lastFrames) / dt);
    lastFrames = frames;
  }
}

char *demo_state_has_fps(void)
{
  char * str = fps_strptr;
  fps_strptr = 0;
  
  return str;
}

void demo_state_next_frame(void)
{
  frames++;
}

static void do_FPS_task(void)
{
  const float dt = task_elapsed(FPS_task) / 1000.0f;
  const float fps = (float)frames / dt;

  printf("%d frames in %3.1f seconds = %3.1f FPS\n", frames, dt, fps);
  lastFrames = lastFrames - frames;
  frames = 0;
}

static void key_angleVel_down(void)
{
  key_input_set_update(change_angleVel, -10.0f);
}

static void key_angleVel_up(void)
{
  key_input_set_update(change_angleVel, 10.0f);
}

static void key_angleVel_pause(void)
{
  update_angleVel(0.0);
}

void demo_state_init(void)
{
  //state->rate = 1.0f;
  state->avgfps = 50.0f;
  state->period_rate = 1.0f / state->avgfps;
  state->angleVel = -30.0f;
  state->LightSourcePosition[0] = -8.0;
  state->LightSourcePosition[1] = 5.0;
  state->LightSourcePosition[2] = 15.0;
  state->LightSourcePosition[3] = 1.0;
  state->LightDirty = GL_TRUE;
  state->instances = 1;
  update_angleFrame();
  
  frames = lastFrames = 0;
  
  FPS_task = task_create();
  task_set_action(FPS_task, do_FPS_task);
  task_set_interval(FPS_task, 5000);
  
  AngleFrame_task = task_create();
  task_set_action(AngleFrame_task, do_AngleFrame_task);
  task_set_interval(AngleFrame_task, 100);
  
  key_input_set_update(0, 0.0f);
  key_add_action('b', demo_state_toggle_VBO, "toggle use of Buffer Objects for gear vertex data");
  key_add_action('I', inc_instances, "add another draw instance of the gears");
  key_add_action('O', dec_instances, "remove an instance of the gears");
  key_add_action('<', key_angleVel_down, "decrease gear spin rate");
  key_add_action('>', key_angleVel_up, "increase gear spin rate");
  key_add_action('p', key_angleVel_pause, "stop gear spin");
  
  atexit(demo_state_delete);

}
