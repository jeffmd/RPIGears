// demo_state_ui.c

#include "gles3.h"

#include "key_input.h"
#include "demo_state.h"

static void demo_state_toggle_VBO_key(const short souce_id, const short destination_id)
{
  DS_toggle_VBO();
}

static void inc_instances(const short souce_id, const short destination_id)
{
  DS_inc_instances();
}

static void dec_instances(const short souce_id, const short destination_id)
{
  DS_dec_instances();
}

static void key_angleVel_down(const short souce_id, const short destination_id)
{
  Key_input_set_update(DS_change_angleVel, -10.0f);
}

static void key_angleVel_up(const short souce_id, const short destination_id)
{
  Key_input_set_update(DS_change_angleVel, 10.0f);
}

static void key_angleVel_pause(const short souce_id, const short destination_id)
{
  DS_angleVel_pause();
}

void DS_ui_init(void)
{
  Key_input_set_update(0, 0.0f);
  Key_add_action('b', demo_state_toggle_VBO_key, "toggle use of Buffer Objects for gear vertex data");
  Key_add_action(SHIFT_KEY('I'), inc_instances, "add another draw instance of the gears");
  Key_add_action(SHIFT_KEY('O'), dec_instances, "remove an instance of the gears");
  Key_add_action(SHIFT_KEY('<'), key_angleVel_down, "decrease gear spin rate");
  Key_add_action(SHIFT_KEY('>'), key_angleVel_up, "increase gear spin rate");
  Key_add_action('p', key_angleVel_pause, "stop gear spin");
}

