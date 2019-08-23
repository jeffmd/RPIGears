/*
* tasks.h
*/

#ifndef _TASKS_H_
  #define _TASKS_H_

  typedef void (*Action)(void);

  int task_create(void);
  void task_set_action(const int id, Action dofunc);
  void task_set_interval(const int id, const uint interval);
  void task_pause(const int id);
  void task_run(const int id);
  uint task_elapsed(const int id);
  uint getMilliseconds();

  void do_tasks(void);
#endif
