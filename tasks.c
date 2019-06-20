/*
 * tasks.c  
 */ 

#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#include "static_array.h"

#define TASKS_MAX_COUNT 20
#define TASKS_DTIME 10
#define TASKS_DO_COUNT TASKS_MAX_COUNT / 4

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
static uint prev_ms;
static uint tasks_dtime = TASKS_DTIME;
static int task_id = 0;

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
  const uint new_dtime = interval / 4;
  if (new_dtime < tasks_dtime) {
    tasks_dtime = new_dtime;
  }
}

uint task_elapsed(Task * const task)
{
  return task->elapsed_ms;
}


static void task_do(Task * const task)
{
  if (task->active) {
    
    if (task->state == TS_RUN) {
      task->elapsed_ms = current_ms - task->prev_ms;
      
      if (task->elapsed_ms >= task->interval_ms) {
        if (task->dofunc) {
          task->dofunc();
        }
        task->prev_ms = current_ms;
      }
    }
  }  
}

void task_pause(Task * const task)
{
  task->state = TS_PAUSED;
}

void task_run(Task * const task)
{
  task->state = TS_RUN;
}

static int next_task_id(void)
{
  if (task_id >= TASKS_MAX_COUNT) {
    task_id = 0;
  }
  
  return task_id++;
}

void do_tasks(void)
{
  update_current_ms();
  if ( (current_ms - prev_ms) > tasks_dtime) {
    prev_ms = current_ms;
      
    for (int idx = 0; idx < TASKS_DO_COUNT; idx++) {
      task_do(tasks + next_task_id());
    }
  }
  
}

