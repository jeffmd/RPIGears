/*
* image.h
*/

#ifndef _IMAGE_H_
  #define _IMAGE_H_
  
  typedef struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[256 * 256 * 3 + 1];
  } IMAGE_T;
  
#endif
