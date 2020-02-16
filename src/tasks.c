/*
 * tasks.c  
 */ 

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "static_array.h"

#define TASKS_MAX_COUNT 10
#define TASKS_DTIME 10
#define TASKS_DO_COUNT TASKS_MAX_COUNT

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
static short next_deleted_task;

static uint current_ms; // current time in milliseconds
static uint prev_ms;
static uint tasks_dtime = TASKS_DTIME;

uint getMilliseconds()
{
  struct timespec spec;

  clock_gettime(CLOCK_REALTIME, &spec);
	
  return (spec.tv_sec * 1000L + spec.tv_nsec / 1000000L);
}
 
static void update_current_ms(void)
{
  current_ms = getMilliseconds();
}

static inline int find_deleted_task_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_task, tasks,
                            TASKS_MAX_COUNT, Task, "task");
}

static Task *get_task(int id)
{
  if ((id < 0) | (id >= TASKS_MAX_COUNT)) {
    id = 0;
    printf("ERROR: Bad Task id, using default id: 0\n");
  }
    
  return tasks + id;
}


static void task_init(Task * const task)
{
  task->prev_ms = getMilliseconds();
  task->elapsed_ms = 0.0f;
}

int task_create(void)
{
  const int id = find_deleted_task_id();
  Task * const task = get_task(id);

  task->active = 1;
  task_init(task);

	return id;
}


void task_set_action(const int id, Action dofunc)
{
  get_task(id)->dofunc = dofunc;
}

void task_set_interval(const int id, const uint interval)
{
  get_task(id)->interval_ms = interval;
  const uint new_dtime = interval / 2;
  if (new_dtime < tasks_dtime) {
    tasks_dtime = new_dtime;
  }
}

uint task_elapsed(const int id)
{
  return get_task(id)->elapsed_ms;
}

static void task_do(Task * const task)
{
  if (task->active) {
    
    if (task->state == TS_RUN) {
      task->elapsed_ms = current_ms - task->prev_ms;
      
      if (task->elapsed_ms >= task->interval_ms) {
        task->prev_ms = current_ms;
        if (task->dofunc) {
          task->dofunc();
        }
      }
    }
  }  
}

void task_pause(const int id)
{
  get_task(id)->state = TS_PAUSED;
}

void task_run(const int id)
{
  get_task(id)->state = TS_RUN;
}

void do_tasks(void)
{
  update_current_ms();
  if ( (current_ms - prev_ms) >= tasks_dtime) {
    prev_ms = current_ms;
      
    for (int idx = 1; idx < TASKS_MAX_COUNT; idx++) {
      task_do(tasks + idx);
    }
  }  
}

