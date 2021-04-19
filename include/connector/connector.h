// connector.h - 
#ifndef _CONECTOR_H_
  #define _CONECTOR_H_

  short Connector_create(const ID_t source_class, const ID_t destination_class, const short pin_count);
  void Connector_set_pin_action(const ID_t connector_id, const ID_t pin_id, ActionFn action);
  short Connector_register_class(const char *name);
  Plug_t Connector_plug(const ID_t connector, const ID_t destination_id);
  void Connector_plug_execute(const Plug_t plug, const ID_t pin_id, const ID_t source_id);

#endif