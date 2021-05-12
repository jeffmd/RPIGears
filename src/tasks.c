/*
 * tasks.c  
 */ 

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "id_plug.h"
#include "static_array.h"

#define TASKS_MAX_COUNT 20
#define TASKS_DO_COUNT TASKS_MAX_COUNT
#define TASKS_DTIME 100

typedef enum
{
  TS_RUN,  
  TS_PAUSED
} TASKSTATE;

typedef void (*Action)(void);

typedef struct
{
  uint8_t active;           // zero if deleted
  uint prev_ms;             // milliseconds when task last executed
  uint next_ms;             // when the task should execute in milliseconds
  uint interval_ms;         // how often in milliseconds the task should run
  TASKSTATE state;          // state of the task ie TS_RUN, TS_PAUSED
  //const char *name;
  Action dofunc;
  
} Task;

static Task tasks[TASKS_MAX_COUNT];
static ID_t next_deleted_task;

static uint current_ms; // current time in milliseconds
static uint prev_ms;
static uint tasks_dtime = TASKS_DTIME;

uint getMilliseconds()
{
  struct timespec spec;

  clock_gettime(CLOCK_MONOTONIC_RAW, &spec);
	
  return (spec.tv_sec * 1000L + spec.tv_nsec / 1000000L);
}
 
static void update_current_ms(void)
{
  current_ms = getMilliseconds();
}

static inline ID_t find_deleted_task_id(void)
{
  return ARRAY_FIND_DELETED_ID(next_deleted_task, tasks,
                            TASKS_MAX_COUNT, Task, "task");
}

static Task *get_task(ID_t id)
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
  task->next_ms = task->prev_ms;
}

static void update_tasks_dtime(const uint interval)
{
  if (interval && (interval < tasks_dtime)) {
    tasks_dtime = interval;
    //printf("tasks dtime: %i\n", interval);
  }
}

static void reset_tasks_dtime(void)
{
  tasks_dtime = TASKS_DTIME;
}

static void task_set_interval_ms(Task *task, const uint interval)
{
  task->interval_ms = interval;
  update_tasks_dtime(interval / 2);
  task->next_ms = getMilliseconds() + interval;
}

ID_t Task_create(const uint interval, Action dofunc)
{
  const ID_t id = find_deleted_task_id();
  Task * const task = get_task(id);

  task->active = 1;
  task_init(task);

  task->dofunc = dofunc;
  task_set_interval_ms(task, interval);
  
  return id;
}

void Task_set_action(const ID_t id, Action dofunc)
{
  get_task(id)->dofunc = dofunc;
}

void Task_set_interval(const ID_t id, const uint interval)
{
  task_set_interval_ms(get_task(id), interval);
}

uint Task_elapsed(const ID_t id)
{
  return current_ms - get_task(id)->prev_ms;
}

static void task_update_next_ms(Task * const task)
{
  task->prev_ms = current_ms;
  task->next_ms = current_ms + task->interval_ms;
}

static void task_execute(Task * const task)
{
  if (task->active) {
    
    if (task->state == TS_RUN) {

      if (current_ms >= task->next_ms) {

        if (task->dofunc) {
          task->dofunc();
        }

        task_update_next_ms(task);
      }

      update_tasks_dtime(task->next_ms - current_ms);
    }
  }  
}

void Task_pause(const ID_t id)
{
  get_task(id)->state = TS_PAUSED;
}

void Task_run(const ID_t id)
{
  Task * const task = get_task(id);

  task->state = TS_RUN;
  task_update_next_ms(task);
}

void Task_do(void)
{
  update_current_ms();
  const uint dtime = current_ms - prev_ms;

  if (dtime < tasks_dtime) {
    usleep((tasks_dtime - dtime) * 1000);
    update_current_ms();
  }

  prev_ms = current_ms;
  reset_tasks_dtime();

  for (int idx = 1; idx < TASKS_MAX_COUNT; idx++) {
    task_execute(tasks + idx);
  }
}

