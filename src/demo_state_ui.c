// demo_state_ui.c

#include <stdio.h>

#include "gles3.h"

#include "id_plug.h"
#include "connector.h"
#include "key_input.h"
#include "demo_state.h"
#include "ui_widget_connector.h"
#include "ui_checkbox.h"
#include "ui_number.h"

enum DS_Field {
  DSF_vbo,
  DSF_instances,
  DSF_rpm
};

static ID_t ds_ui_connector;
static ID_t ds_ui_class;

static const char vbo_str[] = "VBO";
static Plug_t vbo_ui_checkbox;

static const char instances_str[] = "Instances";
static Plug_t instances_ui_number;

static const char rpm_str[] = "Gear RPM";
static Plug_t rpm_ui_number;

static void vbo_toggle(const ID_t souce_id, const ID_t destination_id)
{
  DS_toggle_VBO();
}

static void inc_instances(const ID_t souce_id, const ID_t destination_id)
{
  DS_change_instances(1);
}

static void dec_instances(const ID_t souce_id, const ID_t destination_id)
{
  DS_change_instances(-1);
}

static void key_dec_angleVel(const ID_t souce_id, const ID_t destination_id)
{
  Key_input_set_update(DS_change_angleVel, -10.0f);
}

static void key_inc_angleVel(const ID_t souce_id, const ID_t destination_id)
{
  Key_input_set_update(DS_change_angleVel, 10.0f);
}

static void key_angleVel_pause(const ID_t souce_id, const ID_t destination_id)
{
  DS_angleVel_pause();
}

static ID_t get_ds_ui_class(void)
{
  if (!ds_ui_class) {
    ds_ui_class = Connector_register_class("demo_state_ui");
  }

  return ds_ui_class;
}

static void ds_ui_update(const ID_t source_id, const ID_t destination_id)
{
  switch (destination_id) {
    case DSF_vbo:
      UI_checkbox_update_select(source_id, DS_use_VBO());
      break;

    case DSF_instances:
      UI_number_update_int(source_id, DS_instances());
      break;

    case DSF_rpm:
      UI_number_update_float(source_id, DS_angleVel());
      break;
  }
}

static void ds_ui_changed(const ID_t source_id, const ID_t destination_id)
{
  switch (destination_id) {
    case DSF_vbo:
      vbo_toggle(source_id, destination_id);
      break;

    case DSF_instances:
      DS_set_instances(UI_number_int_new_val(source_id));
      break;

    case DSF_rpm:
      DS_set_angleVel(UI_number_float_new_val(source_id));
      break;
  }
}

static ID_t get_ds_ui_connector(void)
{
  if (!ds_ui_connector) {
    ds_ui_connector = UI_widget_connector(get_ds_ui_class());
    UI_widget_connect_changed(ds_ui_connector, ds_ui_changed);
    UI_widget_connect_update(ds_ui_connector, ds_ui_update);
  }

  return ds_ui_connector;
}

static const Plug_t get_ds_ui_plug(const short field)
{
  return Connector_plug(get_ds_ui_connector(), field);
}

Plug_t DS_ui_vbo(void)
{
  if (!vbo_ui_checkbox) {
    vbo_ui_checkbox = UI_checkbox_create(vbo_str, get_ds_ui_plug(DSF_vbo));
  }

  return vbo_ui_checkbox;
}

Plug_t DS_ui_instances(void)
{
  if (!instances_ui_number) {
    instances_ui_number = UI_number_create(instances_str, get_ds_ui_plug(DSF_instances));
    UI_number_set_edit(instances_ui_number, 1);
  }

  return instances_ui_number;
}

Plug_t DS_ui_rpm(void)
{
  if (!rpm_ui_number) {
    rpm_ui_number = UI_number_create(rpm_str, get_ds_ui_plug(DSF_rpm));
    UI_number_set_edit(rpm_ui_number, 1);
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

