// exit.c

#include <stdlib.h>

#include "tasks.h"

static Task *exit_task;
static int exit_now;

void exit_enable(void)
{
  task_run(exit_task);
  task_set_interval(exit_task, 0);
}

static void exit_task_do(void)
{
  exit_now = 1;
}

int exit_is_now(void)
{
  return exit_now;
}

void exit_init(int time_to_run)
{
  exit_now = 0;  
  exit_task = task_create();
  task_set_action(exit_task, exit_task_do);
  
  if (time_to_run > 0) {
    task_run(exit_task);
    task_set_interval(exit_task, time_to_run);
  }
  else {
    task_pause(exit_task);
  }
}