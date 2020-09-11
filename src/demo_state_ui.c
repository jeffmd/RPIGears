// demo_state_ui.c

#include "gles3.h"

#include "key_input.h"
#include "demo_state.h"
#include "connector.h"
#include "ui_widget_connector.h"
#include "ui_checkbox.h"
#include "ui_number.h"

static const char vbo_str[] = "VBO";
static int vbo_ui_checkbox;
static short vbo_ui_checkbox_connector;
static short demo_state_ui_class;

static const char instances_str[] = "Instances";
static int instances_ui_number;
static short instances_ui_number_connector;

static void vbo_toggle(const short souce_id, const short destination_id)
{
  DS_toggle_VBO();
}

static void vbo_update(const short source_id, const short destination_id)
{
  UI_checkbox_update_select(source_id, DS_use_VBO());
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

static short get_demo_state_ui_class(void)
{
  if (!demo_state_ui_class) {
    demo_state_ui_class = Connector_register_class("demo_state_ui");
  }

  return demo_state_ui_class;
}

static short get_vbo_ui_checkbox_connector(void)
{
  if (!vbo_ui_checkbox_connector) {
    vbo_ui_checkbox_connector = UI_widget_connector(get_demo_state_ui_class());
    UI_widget_connect_change(vbo_ui_checkbox_connector, vbo_toggle);
    UI_widget_connect_update(vbo_ui_checkbox_connector, vbo_update);
  }

  return vbo_ui_checkbox_connector;
}

static const int get_vbo_checkbox_handle(void)
{
  return Connector_handle(get_vbo_ui_checkbox_connector(), 0);
}

int DS_ui_vbo(void)
{
  if (!vbo_ui_checkbox) {
    vbo_ui_checkbox = UI_checkbox_create(vbo_str, get_vbo_checkbox_handle());
  }

  return vbo_ui_checkbox;
}

static void instances_update(const short source_id, const short destination_id)
{
  UI_number_update_int(source_id, DS_instances());
}

static void instances_change(const short source_id, const short destination_id)
{
  if (UI_number_int_change(source_id) > 0) {
    DS_inc_instances();
  }
  else {
    DS_dec_instances();
  }
}

static short get_instances_ui_number_connector(void)
{
  if (!instances_ui_number_connector) {
    instances_ui_number_connector = UI_widget_connector(get_demo_state_ui_class());
    UI_widget_connect_change(instances_ui_number_connector, instances_change);
    UI_widget_connect_update(instances_ui_number_connector, instances_update);
  }

  return instances_ui_number_connector;
}

static const int get_instances_ui_number_handle(void)
{
  return Connector_handle(get_instances_ui_number_connector(), 0);
}

int DS_ui_instances(void)
{
  if (!instances_ui_number) {
    instances_ui_number = UI_number_create(instances_str, get_instances_ui_number_handle());
    UI_number_edit_on(instances_ui_number);
  }

  return instances_ui_number;
}

void DS_ui_init(void)
{
  Key_input_set_update(0, 0.0f);
  Key_add_action('b', vbo_toggle, "toggle use of Buffer Objects for gear vertex data");
  Key_add_action(SHIFT_KEY('I'), inc_instances, "add another draw instance of the gears");
  Key_add_action(SHIFT_KEY('O'), dec_instances, "remove an instance of the gears");
  Key_add_action(SHIFT_KEY('<'), key_angleVel_down, "decrease gear spin rate");
  Key_add_action(SHIFT_KEY('>'), key_angleVel_up, "increase gear spin rate");
  Key_add_action('p', key_angleVel_pause, "stop gear spin");
}

