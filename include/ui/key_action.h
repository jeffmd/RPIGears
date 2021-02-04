// key_action.h

#ifndef _KEY_ACTION_H_
  #define _KEY_ACTION_H_

  typedef void (*ActionFn)(const ID_t souce_id, const ID_t destination_id);

  ID_t Key_Action_create(const int key, ActionFn action, const char* help);
  int Key_Action_key(const ID_t key_action_id);
  void Key_Action_execute(const ID_t key_action_id, const ID_t source_id,  const ID_t destination_id);
  void Key_Action_print_help(void);
#endif