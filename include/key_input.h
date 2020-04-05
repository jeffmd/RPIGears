/*
*  key_input.h
*/

#ifndef _KEY_INPUT_H_
  #define _KEY_INPUT_H_
  
  typedef void (*ActionFn)(const short souce_id, const short destination_id);

  typedef void (*UPDATE_KEY_DOWN)(const float);
  
  void key_input_init(void);
  void key_input_action(const int inpkey);
  void key_add_action(const int key, ActionFn action, const char *help);
  void key_input_print_help(void);
  void key_input_set_update(UPDATE_KEY_DOWN fn, const float val);
  void key_input_down_update(void);
  void key_input_set_rate_frame(const float period_rate);
  void key_input_rate_on(void);
  void key_input_rate_off(void);
  void key_input_inc_rate(void);
  
#endif
