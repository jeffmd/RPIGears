/*
* demo_state.h
*/
#ifndef _demo_state_h_
#define _demo_state_h_ 1


typedef struct {
  GLfloat pos[3];
  GLfloat norm[3];
  GLfloat texCoords[2];
} vertex_t;

typedef struct {
  vertex_t *vertices;
  GLshort *indices;
  GLfloat color[4];
  int nvertices, nindices;
  GLuint vboId; // ID for vertex buffer object
  GLuint iboId; // ID for index buffer object
  
  GLuint tricount; // number of triangles to draw
  GLvoid *vertex_p; // offset or pointer to first vertex
  GLvoid *normal_p; // offset or pointer to first normal
  GLvoid *index_p;  // offset or pointer to first index
  GLvoid *texCoords_p;  // offset or pointer to first texcoord
} gear_t;

typedef void (*UPDATE_KEY_DOWN)(const float);

typedef struct
{
   uint32_t screen_width;
   uint32_t screen_height;
   float move_rate;
   int move_rate_enabled;
   float move_direction;
   float rate_frame;
   UPDATE_KEY_DOWN key_down_update;
// OpenGL|ES objects
   EGLDisplay display;
   EGLSurface surface;
   EGLContext contextGLES1;
   EGLContext contextGLES2;
   GLenum drawMode;
// EGL info
   int major;
   int minor;
// current distance from camera
   GLfloat viewDist;
   GLfloat view_inc;
   GLfloat viewX;
   GLfloat viewY;
// number of seconds to run the demo
   uint timeToRun;
   GLuint texId;

   gear_t *gear1, *gear2, *gear3;
   
// The location of the shader uniforms 
   GLuint ModelViewProjectionMatrix_location,
      ModelViewMatrix_location,
      NormalMatrix_location,
      LightSourcePosition_location,
      MaterialColor_location,
      DiffuseMap_location;
// The projection matrix
   GLfloat ProjectionMatrix[16];
   
// current angle of the gear
   GLfloat angle;
// the degrees that the angle should change each frame
   GLfloat angleFrame;
// the degrees per second the gear should rotate at
   GLfloat angleVel;
// Average Frames Per Second
   float avgfps;
   // the average time between each frame update = 1/avgfps
   float period_rate;
   int useVBO;
   int useGLES2;
   int useVSync;
   int wantInfo;

	 EGL_DISPMANX_WINDOW_T nativewindow;
	 DISPMANX_ELEMENT_HANDLE_T dispman_element;
	 DISPMANX_DISPLAY_HANDLE_T dispman_display;
	 VC_RECT_T dst_rect;
	 VC_RECT_T src_rect;
   float pos_x;
   float pos_y;
   float width;
   float height;
   int window_update;
} DEMO_STATE_T;

#endif
