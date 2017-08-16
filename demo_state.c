/*
* demo_state.c
*/


static DEMO_STATE_T _state, *state = &_state;

void update_timeToRun(uint val)
{
  state->timeToRun = val; 
}

void update_angleVel(GLfloat val)
{
  state->angleVel = val;
}

static void update_avgfps(const float fps)
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
  state->rate_frame = state->rate * state->period_rate;
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
  state->rate_enabled = 1;
}

static void move_rate_off(void)
{
  state->rate_enabled = 0;
  state->rate = 1.0f;
  state->key_down_update = 0;
}

static void inc_move_rate(void)
{
  // increase movement speed if not at max
  //if (state->move_rate < 40.0f)
  if (state->rate_enabled) state->rate += 10 * state->period_rate;
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
  state->rate = 1.0f;
  state->avgfps = 300.0f;
  state->period_rate = 1.0f / state->avgfps;
  state->angleVel = ANGLEVEL;
  
  update_angleFrame();
}
