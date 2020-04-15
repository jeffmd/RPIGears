// handler.h

#ifndef _HANDLER_H_
  #define _HANDLER_H_

  int Handler_create(void);
  void Handler_execute(const short handler_id, const short slot_id, const short source_id);
  void Handler_set_action_table(const short handler_id, const short table_id);
  void Handler_set_destination(const short handler_id, const short destination_id);
#endif