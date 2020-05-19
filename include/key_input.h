/*
*  key_input.h
*/

#ifndef _KEY_INPUT_H_
  #define _KEY_INPUT_H_

  #define SHIFTK 1<<0
  #define CTRLK  1<<2
  #define ALTK   1<<3
  #define STATE_KEY(state, key) (state << 8) + key
  #define SHIFT_KEY(key) STATE_KEY(SHIFTK, key)
  #define CTRL_KEY(key) STATE_KEY(CTRLK, key)
  #define SHIFT_CTRL_KEY(key) STATE_KEY((SHIFTK | CTRLK), key)
  #define ALT_KEY(key) STATE_KEY(ALTK, key)
  #define SHIFT_ALT_KEY(key) STATE_KEY((SHIFTK | ALTK), key)
  #define CTRL_ALT_KEY(key) STATE_KEY((CTRLK | ALTK), key)
  #define KEY_BUTTON(key) (127 + key) 


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
