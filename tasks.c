/*
 * tasks.c  
 */ 

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "demo_state.h"
#include "key_input.h"

typedef enum
{
  TS_PAUSED,
  TS_RUN  
} TASKSTATE;

typedef struct
{
  uint prev_ms; // when the task last executed in milliseconds
  uint interval_ms; // how often in milliseconds the task should run
  uint elapsed_ms; // how many milliseconds since the last time the task executed
  TASKSTATE state; // state of the task ie TS_RUN, TS_PAUSED
  
} Task_T;

static uint current_ms; // current time in milliseconds

static uint getMilliseconds()
{
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
	
	return (spec.tv_sec * 1000L + spec.tv_nsec / 1000000L);
}

 
static void update_current_ms(void)
{
  current_ms = getMilliseconds();
}


typedef void (*Action)(void);

static int run_task(Task_T * const task, Action dofunc)
{
  int is_ready = 0;
  
  if (task->state == TS_RUN) {
    task->elapsed_ms = current_ms - task->prev_ms;
    
    is_ready = task->elapsed_ms > task->interval_ms;
    if (is_ready) {
      if (dofunc) dofunc();
      task->prev_ms = current_ms;
    }
  }
    
  return is_ready;
}


static void reset_task(Task_T * const task)
{
  task->prev_ms = getMilliseconds();
  task->elapsed_ms = 0.0f;
}


static int frames; // number of frames drawn since the last frame/sec calculation

static Task_T AngleFrame_task = { 0, 500, 0, TS_RUN};
static Task_T FPS_task = {0, 5000, 0, TS_RUN};
static Task_T KeyScan_task = {0, 40, 0, TS_RUN};
static Task_T Exit_task = {0, 0, 0, TS_PAUSED};

static void do_AngleFrame_task(void)
{
  const float dt = FPS_task.elapsed_ms / 1000.0f;
  if (dt > 0.0f) {
    update_avgfps((float)frames / dt);
    update_angleFrame();
    update_rate_frame();
  }
}

static void do_FPS_task(void)
{
  const float dt = FPS_task.elapsed_ms / 1000.0f;
  const float fps = (float)frames / dt;
  printf("%d frames in %3.1f seconds = %3.1f FPS\n", frames, dt, fps);
  frames = 0;
}

static void init_Exit_task(void)
{
  reset_task(&Exit_task);
  Exit_task.interval_ms = state_timeToRun();
  if (Exit_task.interval_ms > 0.0f) Exit_task.state = TS_RUN;
}

static void do_Exit_task(void)
{
  Exit_task.state = TS_RUN;
  Exit_task.interval_ms = 0.0f;
}

static void do_KeyScan_task(void)
{
  switch (detect_keypress())
  {
    // stop the program if a special key was hit
    case 0: do_Exit_task(); break;
    // speed up key processing if more keys in buffer
    case 2: KeyScan_task.interval_ms = 10.0f; break;
    default: KeyScan_task.interval_ms = 100.0f;
  }
}

void reset_tasks(void)
{
  frames = 0;
  reset_task(&FPS_task);
  reset_task(&AngleFrame_task);
  reset_task(&KeyScan_task);
  init_Exit_task();

  update_current_ms();
}

int run_exit_task(void)
{
  return run_task(&Exit_task, 0);
}

void do_tasks(void)
{
  frames++;
  update_current_ms();
  // print out fps stats every 5 secs
  run_task(&FPS_task, do_FPS_task);
  // adjust angleFrame each half second
  run_task(&AngleFrame_task, do_AngleFrame_task);
  // about every 40ms check if the user hit the keyboard
  run_task(&KeyScan_task, do_KeyScan_task);
}

