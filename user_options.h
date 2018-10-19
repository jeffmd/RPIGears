/*
* user_options.h
*/

int options_useVBO(void);
int options_useVSync(void);
int options_wantInfo(void);
GLenum options_drawMode(void);
GLfloat options_angleVel(void);
uint options_timeToRun(void);
int setup_user_options(int argc, char *argv[]);
void toggle_drawmode(void);
void update_useVSync(const int sync);
