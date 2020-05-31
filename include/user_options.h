/*
* user_options.h
*/
#ifndef _USER_OPTIONS_H_
  #define _USER_OPTIONS_H_
  
  int Options_useVBO(void);
  int Options_useVSync(void);
  int Options_wantInfo(void);
  GLenum Options_drawMode(void);
  GLfloat Options_angleVel(void);
  GLuint Options_timeToRun(void);
  int setup_user_options(int argc, char *argv[]);
  void Options_toggle_drawmode(void);
  void Options_update_useVSync(const int sync);
#endif
