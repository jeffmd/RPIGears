/*
* tasks.h
*/

#ifndef _TASKS_H_
  #define _TASKS_H_

  typedef void (*Action)(void);

  short task_create(void);
  void task_set_action(const short id, Action dofunc);
  void task_set_interval(const short id, const uint interval);
  void task_pause(const short id);
  void task_run(const short id);
  uint task_elapsed(const short id);
  uint getMilliseconds();

  void do_tasks(void);
#endif
