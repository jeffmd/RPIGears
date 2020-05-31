// exit.c

#include <stdlib.h>

#include "tasks.h"
#include "key_input.h"

static short exit_task;
static short exit_now;
static const char exit_help[] = "esc or Enter - end program";

void Exit_enable(void)
{
  Task_run(exit_task);
}

static void exit_enable_key(const short souce_id, const short destination_id)
{
  Exit_enable();
}

static void exit_task_do(void)
{
  exit_now = 1;
}

int Exit_is_now(void)
{
  return exit_now;
}

void Exit_init(const int time_to_run)
{
  exit_now = 0;  
  exit_task = Task_create(time_to_run, exit_task_do);
  
  if (time_to_run > 0) {
    Task_run(exit_task);
  }
  else {
    Task_pause(exit_task);
  }
  
  Key_add_action(10, exit_enable_key, 0);
  Key_add_action(13, exit_enable_key, 0);
  Key_add_action(27, exit_enable_key, exit_help);
}
