// user_options_ui.c

#include <stdio.h>
#include <stdlib.h>

#include "gles3.h"
#include "user_options.h"
#include "window.h"
#include "key_input.h"
#include "ui_checkbox.h"
#include "connector.h"

static const char vsync_str[] = "vsync";
static int vsync_ui_checkbox;
static short vsync_ui_checkbox_connector;
static short user_options_ui_class;

static void vsync_toggle(const short souce_id, const short destination_id)
{
  const int sync = User_Options_useVSync() ? 0 : 1;
  User_Options_update_useVSync(sync);
  Window_update_VSync(sync);
  printf("\nvertical sync is %s\n", sync ? "on": "off");
}

static void drawmode_toggle(const short souce_id, const short destination_id)
{
  User_Options_toggle_drawmode();
}

static short get_user_options_ui_class(void)
{
  if (!user_options_ui_class) {
    user_options_ui_class = Connector_register_class("user_options_ui");
  }

  return user_options_ui_class;
}

static short get_vsync_ui_checkbox_connector(void)
{
  if (!vsync_ui_checkbox_connector) {
    vsync_ui_checkbox_connector = UI_checkbox_connector(get_user_options_ui_class());
    UI_checkbox_connect_select(vsync_ui_checkbox_connector, vsync_toggle);
    //UI_checkbox_connect_update(user_options_checkbox_connector, vsync_update);
  }

  return vsync_ui_checkbox_connector;
}

static const int get_vsync_checkbox_handle(void)
{
  return Connector_handle(get_vsync_ui_checkbox_connector(), 0);
}

int User_options_ui_vsync(void)
{
  if (!vsync_ui_checkbox) {
    vsync_ui_checkbox = UI_checkbox_create(vsync_str, get_vsync_checkbox_handle());
  }

  return vsync_ui_checkbox;
}

void User_options_ui_init(void)
{
  Key_add_action('v', vsync_toggle, "toggle vertical sync on/off");
  Key_add_action('l', drawmode_toggle, "toggle draw mode: GL_TRIANGLES / GL_LINES / GL_POINT");
}