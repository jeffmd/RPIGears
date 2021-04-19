// user_options_ui.c

#include <stdio.h>

#include "gles3.h"
#include "id_plug.h"
#include "user_options.h"
#include "window.h"
#include "key_input.h"
#include "connector.h"
#include "ui_widget_connector.h"
#include "ui_checkbox.h"

static const char vsync_str[] = "vsync";
static Plug_t vsync_ui_checkbox;
static ID_t vsync_ui_checkbox_connector;
static ID_t user_options_ui_class;

static void vsync_toggle(const ID_t source_id, const ID_t destination_id)
{
  const int sync = User_Options_useVSync() ? 0 : 1;
  User_Options_update_useVSync(sync);
  Window_update_VSync(sync);
  printf("\nvertical sync is %s\n", sync ? "on": "off");
}

static void vsync_update(const ID_t checkbox_id, const ID_t destination_id)
{
  UI_checkbox_update_select(checkbox_id, User_Options_useVSync());
}

static void drawmode_toggle(const ID_t source_id, const ID_t destination_id)
{
  User_Options_toggle_drawmode();
}

static ID_t get_user_options_ui_class(void)
{
  if (!user_options_ui_class) {
    user_options_ui_class = Connector_register_class("user_options_ui");
  }

  return user_options_ui_class;
}

static ID_t get_vsync_ui_checkbox_connector(void)
{
  if (!vsync_ui_checkbox_connector) {
    vsync_ui_checkbox_connector = UI_widget_connector(get_user_options_ui_class());
    UI_widget_connect_changed(vsync_ui_checkbox_connector, vsync_toggle);
    UI_widget_connect_update(vsync_ui_checkbox_connector, vsync_update);
  }

  return vsync_ui_checkbox_connector;
}

static const Plug_t get_vsync_checkbox_plug(void)
{
  return Connector_plug(get_vsync_ui_checkbox_connector(), 0);
}

Plug_t User_options_ui_vsync(void)
{
  if (!vsync_ui_checkbox) {
    vsync_ui_checkbox = UI_checkbox_create(vsync_str, get_vsync_checkbox_plug());
  }

  return vsync_ui_checkbox;
}

void User_options_ui_init(void)
{
  Key_add_action('v', vsync_toggle, "toggle vertical sync on/off");
  Key_add_action('l', drawmode_toggle, "toggle draw mode: GL_TRIANGLES / GL_LINES / GL_POINT");
}

