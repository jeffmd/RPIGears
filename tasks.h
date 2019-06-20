/*
* tasks.h
*/

#ifndef _TASKS_H_
  #define _TASKS_H_

  typedef void (*Action)(void);
  typedef struct Task Task;

  Task *task_create(void);
  void task_set_action(Task * const task, Action dofunc);
  void task_set_interval(Task * const task, uint interval);
  void task_pause(Task * const task);
  void task_run(Task * const task);
  uint task_elapsed(Task * const task);

  char *has_fps(void);
  void do_tasks(void);
  void reset_tasks(void);
#endif
