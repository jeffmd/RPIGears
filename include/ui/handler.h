// handler.h

#ifndef _HANDLER_H_
  #define _HANDLER_H_

  int Handler_create(const short destination_id, const short table_id);
  void Handler_execute(const int handler_id, const short slot_id, const short source_id);
#endif