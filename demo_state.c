/*
* demo_state.c
*/


static void init_window_pos(void)
{
  state->pos_x = (float)state->dst_rect.x;
  state->pos_y = (float)state->dst_rect.y;
}

static void init_window_size(void)
{
  state->width = (float)state->dst_rect.width;
  state->height = (float)state->dst_rect.height;
}

static void set_key_down_update(UPDATE_KEY_DOWN updatefn, const float direction)
{
  state->key_down_update = updatefn;
  state->move_direction = direction;  
}

static void do_key_down_update(void)
{
  if (state->key_down_update) {
    state->key_down_update(state->move_direction * state->rate_frame);
  }
}

static void update_avgfps(float fps)
{
  state->avgfps = state->avgfps * 0.4f + fps * 0.6f;
  state->period_rate = 1.0f / state->avgfps;
}

static void update_angleFrame(void)
{
	state->angleFrame = state->angleVel * state->period_rate;
}

static void update_rate_frame(void)
{
  state->rate_frame = state->move_rate * state->period_rate;
}

static void update_useVSync(int sync)
{
  state->useVSync = sync;
  EGLBoolean result = eglSwapInterval(state->display, state->useVSync );
  assert(EGL_FALSE != result);
}

static void toggle_useVSync(void)
{
  int sync = state->useVSync ? 0 : 1;
  update_useVSync(sync);
  printf("\nvertical sync is %s\n", sync ? "on": "off");
}

static void toggle_drawmode(void)
{
  state->drawMode = state->drawMode == GL_TRIANGLES ? GL_LINES : GL_TRIANGLES;
  printf("\ndraw mode is %s\n", state->drawMode == GL_TRIANGLES ? "GL_TRIANGLES": "GL_LINES");
}

static void change_angleVel(const float val)
{
  state->angleVel += val;
}

static void change_viewDist(const float val)
{
  state->viewDist += val;  
}

static void change_viewX(const float val)
{
  state->viewX += val;
}

static void change_viewY(const float val)
{
  state->viewY += val;  
}

static void update_gear_rotation(void)
{
    /* advance gear rotation for next frame */
    state->angle += state->angleFrame;
    if (state->angle > 360.0)
      state->angle -= 360.0;
}

static void move_rate_on(void)
{
  state->move_rate_enabled = 1;
}

static void move_rate_off(void)
{
  state->move_rate_enabled = 0;
  state->move_rate = 1.0f;
  state->key_down_update = 0;
}

static void inc_move_rate(void)
{
  // increase movement speed if not at max
  //if (state->move_rate < 40.0f)
  if (state->move_rate_enabled) state->move_rate += 10 * state->period_rate;
}


static void move_window_x(const float val)
{
  state->pos_x += val;
  state->dst_rect.x = (int)state->pos_x;
  state->window_update = 1;
}


static void move_window_y(const float val)
{
  state->pos_y += val;
  state->dst_rect.y = (int)state->pos_y;
  state->window_update = 1;
}

static void move_window_home(void)
{
  state->dst_rect.x = state->screen_width/4;
  state->dst_rect.y = state->screen_height/4;
  init_window_pos();
  state->window_update = 1;
}

static void move_window_end(void)
{
  state->dst_rect.x = state->screen_width;
  state->dst_rect.y = state->screen_height;
  init_window_pos();
  state->window_update = 1;
}

static void zoom_window(const float val)
{
  state->width += val;
  state->height += val;
  state->dst_rect.width = (int)state->width;
  state->dst_rect.height = (int)state->height;
  state->window_update = 1;
}

static void check_window_offsets(void)
{
  if (state->dst_rect.x <= -state->dst_rect.width) {
    state->dst_rect.x = -state->dst_rect.width + 1;
    state->pos_x = (float)state->dst_rect.x;
  }
  else
    if (state->dst_rect.x > (int)state->screen_width) {
      state->dst_rect.x = (int)state->screen_width;
      state->pos_x = (float)state->dst_rect.x; 
    }
       
  if (state->dst_rect.y <= -state->dst_rect.height) {
    state->dst_rect.y = -state->dst_rect.height + 1;
    state->pos_y = (float)state->dst_rect.y; 
  }  
  else
    if (state->dst_rect.y > (int)state->screen_height) {
       state->dst_rect.y = (int)state->screen_height;
       state->pos_y = (float)state->dst_rect.y;
    }
}

static void init_demo_state(void)
{
  // Clear application state
  memset( state, 0, sizeof( *state ) );
// setup some default states
  state->viewDist = -38.0f;
  state->viewX = -8.0f;
  state->viewY = -7.0f;
  state->view_inc = 0.02f;
  state->move_rate = 1.0f;
  state->avgfps = 300.0f;
  state->period_rate = 1.0f / state->avgfps;
  state->angleVel = ANGLEVEL;
  state->useVBO = 0;
  state->drawMode = GL_TRIANGLES;
  
}
