/*
 * tasks.c  
 */ 

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "gles3.h"
#include "gear.h"
#include "gpu_texture.h"
#include "demo_state.h"
#include "key_input.h"
#include "static_array.h"

#define TASKS_MAX_COUNT 10

typedef enum
{
  TS_RUN,  
  TS_PAUSED
} TASKSTATE;

typedef void (*Action)(void);

typedef struct
{
  uint8_t active;           // zero if deleted
  uint prev_ms;             // when the task last executed in milliseconds
  uint interval_ms;         // how often in milliseconds the task should run
  uint elapsed_ms;          // how many milliseconds since the last time the task executed
  TASKSTATE state;          // state of the task ie TS_RUN, TS_PAUSED
  const char *name;
  Action dofunc;
  
} Task;

static Task tasks[TASKS_MAX_COUNT];
static Task *next_deleted_task = 0;

static uint current_ms; // current time in milliseconds
static char fps_str[12];
static char *fps_strptr;

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

static inline Task *find_deleted_task(void)
{
  return ARRAY_FIND_DELETED(next_deleted_task, tasks,
                            TASKS_MAX_COUNT, "task");
}

static void task_init(Task * const task)
{
  task->prev_ms = getMilliseconds();
  task->elapsed_ms = 0.0f;
}

Task *task_create(void)
{
  Task *task = find_deleted_task();

  task->active = 1;
  task_init(task);

	return task;
}


void task_set_action(Task * const task, Action dofunc)
{
  task->dofunc = dofunc;
}

void task_set_interval(Task * const task, uint interval)
{
  task->interval_ms = interval;
}

static int task_do(Task * const task)
{
  int is_ready = 0;

  if (task->active) {
    
    if (task->state == TS_RUN) {
      task->elapsed_ms = current_ms - task->prev_ms;
      
      is_ready = task->elapsed_ms >= task->interval_ms;
      if (is_ready) {
        if (task->dofunc) task->dofunc();
        task->prev_ms = current_ms;
      }
    }
  }  

  return is_ready;
}

void task_pause(Task * const task)
{
  task->state = TS_PAUSED;
}

void task_run(Task * const task)
{
  task->state = TS_RUN;
}


static int frames; // number of frames drawn since the last frame/sec calculation
static int lastFrames;

static Task *AngleFrame_task;
static Task *FPS_task;
static Task *KeyScan_task;
static Task *Exit_task;

static void do_AngleFrame_task(void)
{
  
  const float dt = AngleFrame_task->elapsed_ms / 1000.0f;
  if (dt > 0.0f) {
	  
    update_avgfps((float)(frames - lastFrames) / dt);
    lastFrames = frames;
    update_angleFrame();
    update_rate_frame();
  }
}

char *has_fps(void)
{
  char * str = fps_strptr;
  fps_strptr = 0;
  
  return str;
}

static void do_FPS_task(void)
{
  const float dt = FPS_task->elapsed_ms / 1000.0f;
  const float fps = (float)frames / dt;
  sprintf(fps_str, "%3.1f", fps);
  fps_strptr = fps_str;
  printf("%d frames in %3.1f seconds = %s FPS\n", frames, dt, fps_str);
  lastFrames = lastFrames - frames;
  frames = 0;
}

static void init_Exit_task(void)
{
  Exit_task = task_create();
  
  if (state_timeToRun() > 0) {
    task_run(Exit_task);
    task_set_interval(Exit_task, state_timeToRun());
  }
  else {
    task_pause(Exit_task);
  }
}

void enable_exit(void)
{
  task_run(Exit_task);
  task_set_interval(Exit_task, 0);
}

static void do_KeyScan_task(void)
{
  switch (detect_keypress())
  {
    // stop the program if a special key was hit
    case 0: enable_exit(); break;
    // speed up key processing if more keys in buffer
    case 2: task_set_interval(KeyScan_task, 10); break;
    default: task_set_interval(KeyScan_task, 100);
  }
}

void reset_tasks(void)
{
  frames = lastFrames = 0;
  
  FPS_task = task_create();
  task_set_action(FPS_task, do_FPS_task);
  task_set_interval(FPS_task, 5000);
  
  AngleFrame_task = task_create();
  task_set_action(AngleFrame_task, do_AngleFrame_task);
  task_set_interval(AngleFrame_task, 100);
  
  KeyScan_task = task_create();
  task_set_action(KeyScan_task, do_KeyScan_task);
  task_set_interval(KeyScan_task, 40);
  
  init_Exit_task();

  update_current_ms();
}

int run_exit_task(void)
{
  return task_do(Exit_task);
}

void do_tasks(void)
{
  frames++;
  update_current_ms();
  // print out fps stats every 5 secs
  for (int idx = 0; idx < TASKS_MAX_COUNT; idx++) {
    task_do(tasks + idx);
  }
}

