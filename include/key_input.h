/*
*  key_input.h
*/

#ifndef _KEY_INPUT_H_
  #define _KEY_INPUT_H_
  
  typedef void (*ActionFn)(const short souce_id, const short destination_id);

  typedef void (*UPDATE_KEY_DOWN)(const float);
  
  void Key_input_init(void);
  void Key_input_action(const int inpkey);
  void Key_add_action(const int key, ActionFn action, const char *help);
  void Key_input_print_help(void);
  void Key_input_set_update(UPDATE_KEY_DOWN fn, const float val);
  void Key_input_down_update(void);
  void Key_input_set_rate_frame(const float period_rate);
  void Key_input_rate_on(void);
  void Key_input_rate_off(void);
  void Key_input_inc_rate(void);
  
#endif
