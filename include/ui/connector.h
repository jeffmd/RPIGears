// connector.h - 
#ifndef _CONECTOR_H_
  #define _CONECTOR_H_

  typedef int Handle_t;
  typedef void (*ActionFn)(const ID_t source_id, const ID_t destination_id);

  short Connector_create(const ID_t source_class, const ID_t destination_class, const short count);
  void Connector_set_action(const ID_t connector_id, const ID_t slot_id, ActionFn action);
  short Connector_register_class(const char *name);
  Handle_t Connector_handle(const ID_t connector, const ID_t destination_id);
  void Connector_handle_execute(const Handle_t handle, const ID_t slot_id, const ID_t source_id);

#endif