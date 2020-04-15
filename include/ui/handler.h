// handler.h

#ifndef _HANDLER_H_
  #define _HANDLER_H_

  short Handler_create(const short destination_id, const short table_id);
  void Handler_execute(const short handler_id, const short slot_id, const short source_id);
#endif