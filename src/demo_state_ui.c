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

static const char rpm_str[] = "Gear RPM";
static int rpm_ui_number;
static short rpm_ui_number_connector;

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
  DS_change_instances(1);
}

static void dec_instances(const short souce_id, const short destination_id)
{
  DS_change_instances(-1);
}

static void key_dec_angleVel(const short souce_id, const short destination_id)
{
  Key_input_set_update(DS_change_angleVel, -10.0f);
}

static void key_inc_angleVel(const short souce_id, const short destination_id)
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
  DS_change_instances(UI_number_int_change(source_id));
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

static void rpm_update(const short source_id, const short destination_id)
{
  UI_number_update_float(source_id, DS_angleVel());
}

static void rpm_change(const short source_id, const short destination_id)
{
  DS_change_angleVel(UI_number_float_change(source_id));
}

static short get_rpm_ui_number_connector(void)
{
  if (!rpm_ui_number_connector) {
    rpm_ui_number_connector = UI_widget_connector(get_demo_state_ui_class());
    UI_widget_connect_change(rpm_ui_number_connector, rpm_change);
    UI_widget_connect_update(rpm_ui_number_connector, rpm_update);
  }

  return rpm_ui_number_connector;
}

static const int get_rpm_ui_number_handle(void)
{
  return Connector_handle(get_rpm_ui_number_connector(), 0);
}

int DS_ui_rpm(void)
{
  if (!rpm_ui_number) {
    rpm_ui_number = UI_number_create(rpm_str, get_rpm_ui_number_handle());
    UI_number_edit_on(rpm_ui_number);
    UI_number_set_default_float_change(rpm_ui_number, 1.0f);
  }

  return rpm_ui_number;
}

void DS_ui_init(void)
{
  Key_input_set_update(0, 0.0f);
  Key_add_action('b', vbo_toggle, "toggle use of Buffer Objects for gear vertex data");
  Key_add_action(SHIFT_KEY('I'), inc_instances, "add another draw instance of the gears");
  Key_add_action(SHIFT_KEY('O'), dec_instances, "remove an instance of the gears");
  Key_add_action(SHIFT_KEY('<'), key_dec_angleVel, "decrease gear spin rate");
  Key_add_action(SHIFT_KEY('>'), key_inc_angleVel, "increase gear spin rate");
  Key_add_action('p', key_angleVel_pause, "stop gear spin");
}

