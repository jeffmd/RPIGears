/*
* tasks.h
*/

#ifndef _TASKS_H_
  #define _TASKS_H_

  typedef void (*Action)(void);
  char *has_fps(void);
  void do_tasks(void);
  void reset_tasks(void);
  int run_exit_task(void);
  void enable_exit(void);
#endif
