/*
 * print_info.c
 */

void print_GLInfo(void)
{
  printf("\nGL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
  printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
  printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
  printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
  printf("EGL version   = %i.%i\n", window_major(), window_minor());
}

void print_keyhelp(void)
{
  printf(
   "\nspecial keys and what they do\n"
   "i - print GL info\n"
   "l - toggle draw mode GL_TRIAGLES/GL_LINES\n"
   "o - print command line options\n"
   "p - toggle spin on/off of gears\n"
   "v - toggle vertical sync on/off\n"
   "z - increase window size (zoom in)\n"
   "Z - decrease window size (zoom out)\n"
   "< - decrease gear spin rate\n"
   "> - increase gear spin rate\n"
   "a - move camera left\n"
   "d - move camera right\n"
   "w - move camera up\n"
   "s - move camera down\n"
   "r - move camera back from gears\n"
   "f - move camera toward gears\n"
   "up arrow - move window up\n"
   "down arrow - move window down\n"
   "left arrow - move window left\n"
   "right arrow - move window right\n"
   "home - move window to centre of screen\n"
   "end - move window off screen\n"
   );
}

void print_CLoptions_help(void)
{
  printf(
    "\nusage: RPIGears [options]\n"
    "options: -vsync | -exit | -info | -vbo | -gles2 | -line | -nospin\n"
    "-vsync: wait for vertical sync before new frame is displayed\n"
    "-exit: automatically exit RPIGears after 30 seconds\n"
    "-info: display opengl driver info\n"
    "-vbo: use vertex buffer object in GPU memory\n"
    "-line: draw lines only, wire frame mode\n"
    "-nospin: gears don't turn\n"
    );  
}

