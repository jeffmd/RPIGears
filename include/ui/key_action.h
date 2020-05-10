// key_action.h

#ifndef _KEY_ACTION_H_
  #define _KEY_ACTION_H_

  typedef void (*ActionFn)(const short souce_id, const short destination_id);

  short Key_Action_create(const short key, ActionFn action, const char* help);
  short Key_Action_key(const short key_action_id);
  void Key_Action_execute(const short key_action_id, const short source_id,  const short destination_id);
  void Key_Action_print_help(void);
#endif