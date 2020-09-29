/*
* tasks.h
*/

#ifndef _TASKS_H_
  #define _TASKS_H_

  typedef void (*Action)(void);

  short Task_create(const uint interval, Action dofunc);
  void Task_set_action(const short id, Action dofunc);
  void Task_set_interval(const short id, const uint interval);
  void Task_pause(const short id);
  void Task_run(const short id);
  uint Task_elapsed(const short id);
  uint getMilliseconds();
  void Task_do(void);

#endif
