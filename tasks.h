/*
* tasks.h
*/

#ifndef _TASKS_H_
  #define _TASKS_H_

  typedef void (*Action)(void);

  int task_create(void);
  void task_set_action(int id, Action dofunc);
  void task_set_interval(int id, uint interval);
  void task_pause(int id);
  void task_run(int id);
  uint task_elapsed(int id);
  uint getMilliseconds();

  void do_tasks(void);
#endif
