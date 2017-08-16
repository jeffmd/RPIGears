/*
* user_options.h
*/

extern int options_useVBO(void);
extern int options_useGLES2(void);
extern int options_useVSync(void);
extern int options_wantInfo(void);
extern GLenum options_drawMode(void);
extern GLfloat options_angleVel(void);
extern uint options_timeToRun(void);
extern int setup_user_options(int argc, char *argv[]);
extern void toggle_drawmode(void);
extern void update_useVSync(const int sync);
