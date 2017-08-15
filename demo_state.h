/*
* demo_state.h
*/

#ifndef _demo_state_h_
  #define _demo_state_h_

 
  // a procedure that takes a value representing the rate change to apply to an update
  typedef void (*UPDATE_KEY_DOWN)(const float);
  
  typedef struct
  {
  // number of seconds to run the demo
     uint timeToRun;
     GLuint texId;
  
     int gear1, gear2, gear3;
  // camera data
     GLfloat viewDist;
     GLfloat view_inc;
     GLfloat viewX;
     GLfloat viewY;
     
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
     
     // keyboard data
     UPDATE_KEY_DOWN key_down_update; // points to a function that gets executed each time a key goes down or repeats
     float rate; // the rate at which a change occurs
     int rate_enabled; // if enabled the change_rate will increase each frame
     float rate_direction; // direction and scale for rate change
     float rate_frame; // how much the rate changes each frame
  } DEMO_STATE_T;


#endif
