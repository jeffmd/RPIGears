// connector.h - 
#ifndef _CONECTOR_H_
  #define _CONECTOR_H_

  typedef void (*ActionFn)(const short source_id, const short destination_id);

  short Connector_create(const short source_class, const short destination_class);
  void Connector_allocate_slots(const short connector_id, const short count);
  void Connector_set_action(const short connector_id, const short slot_id, ActionFn action);
  short Connector_register_class(const char *name);
  int Connector_handle(const short connector, const short destination_id);
  void Connector_handle_execute(const int handle, const short slot_id, const short source_id);

#endif