/*
* window.c
*/

static void init_window_pos(void)
{
  window->pos_x = (float)window->dst_rect.x;
  window->pos_y = (float)window->dst_rect.y;
}

static void init_window_size(void)
{
  window->width = (float)window->dst_rect.width;
  window->height = (float)window->dst_rect.height;
}


static void move_window_x(const float val)
{
  window->pos_x += val;
  window->dst_rect.x = (int)window->pos_x;
  window->update = 1;
}


static void move_window_y(const float val)
{
  window->pos_y += val;
  window->dst_rect.y = (int)window->pos_y;
  window->update = 1;
}

static void move_window_home(void)
{
  window->dst_rect.x = window->screen_width/4;
  window->dst_rect.y = window->screen_height/4;
  init_window_pos();
  window->update = 1;
}

static void move_window_end(void)
{
  window->dst_rect.x = window->screen_width;
  window->dst_rect.y = window->screen_height;
  init_window_pos();
  window->update = 1;
}

static void zoom_window(const float val)
{
  window->width += val;
  window->height += val;
  window->dst_rect.width = (int)window->width;
  window->dst_rect.height = (int)window->height;
  window->update = 1;
}

static void check_window_offsets(void)
{
  if (window->dst_rect.x <= -window->dst_rect.width) {
    window->dst_rect.x = -window->dst_rect.width + 1;
    window->pos_x = (float)window->dst_rect.x;
  }
  else
    if (window->dst_rect.x > (int)window->screen_width) {
      window->dst_rect.x = (int)window->screen_width;
      window->pos_x = (float)window->dst_rect.x; 
    }
       
  if (window->dst_rect.y <= -window->dst_rect.height) {
    window->dst_rect.y = -window->dst_rect.height + 1;
    window->pos_y = (float)window->dst_rect.y; 
  }  
  else
    if (window->dst_rect.y > (int)window->screen_height) {
       window->dst_rect.y = (int)window->screen_height;
       window->pos_y = (float)window->dst_rect.y;
    }
}

