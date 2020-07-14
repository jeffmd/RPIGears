/*
* user_options.h
*/
#ifndef _USER_OPTIONS_H_
  #define _USER_OPTIONS_H_
  
  int User_Options_useVBO(void);
  int User_Options_useVSync(void);
  int User_Options_wantInfo(void);
  GLenum User_Options_drawMode(void);
  GLfloat User_Options_angleVel(void);
  GLuint User_Options_timeToRun(void);
  int User_Options_setup(int argc, char *argv[]);
  void User_Options_toggle_drawmode(void);
  void User_Options_update_useVSync(const int sync);
#endif
