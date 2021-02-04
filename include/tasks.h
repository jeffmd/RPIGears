/*
* tasks.h
*/

#ifndef _TASKS_H_
  #define _TASKS_H_

  typedef void (*Action)(void);

  ID_t Task_create(const uint interval, Action dofunc);
  void Task_set_action(const ID_t id, Action dofunc);
  void Task_set_interval(const ID_t id, const uint interval);
  void Task_pause(const ID_t id);
  void Task_run(const ID_t id);
  uint Task_elapsed(const ID_t id);
  uint getMilliseconds();
  void Task_do(void);

#endif
