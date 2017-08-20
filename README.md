RPIGears
========

OpengGL ES port of the classic gl gears demo for the Raspberry Pi.

Building
--------
The following files should be in the build directory:

* LICENSE
* Makefile
* README.md
* RPIGears.c
* RPi_Logo256.c
* image.h
* camera.c
* camera.h
* matrix_math.c
* matrix_math.h
* key_input.c
* key_input.h
* tasks.c
* tasks.h
* shaders.c
* print_info.c
* print_info.h
* gles1.c
* gles2.c
* gear.c
* gear.h
* scene.c
* user_options.c
* user_options.h
* demo_state.c
* demo_state.h
* window.h
* window.c


In a terminal session, change directory to the build directory where the 
RPIGears source files are located.  Use make at the commandline to build
RPIGears:

`make -j4`

No special libraries/packages are required to build if using Raspbian.


Running
-------

To run the demo type `./RPIGears.bin.`  The demo runs full screen.  To
exit the demo press esc or enter key on the keyboard.


Command line Options
--------------------
usage: ./RPIGears.bin [options]

options: -vsync | -exit | -info | -vbo | -gles2


-vsync : wait for vertical sync before new frame is displayed

-exit  : automatically exit RPIGears after 30 seconds

-info  : display opengl driver info

-vbo   : use vertex buffer object in GPU memory

-gles2 : use opengl es 2.0 rendering path (programable shaders)

-line  : draw lines only, wire frame mode

-nospin: gears don't turn


Options can be used in any combination.


Speed keys while demo is running
-----------------------------
i - print GL info to console

l - toggle draw mode GL_TRIAGLES/GL_LINES/GL_POINTS

o - print command line options to console

v - toggle vertical sync on/off

z - increase window size (zoom in)

Z - decrease window size (zoom out)

< - decrease gear spin rate

> - increase gear spin rate

a - move camera left

d - move camera right

w - move camera up

s - move camera down

r - move camera away from gears

f - move camera toward gears

up arrow - move window up

down arrow - move window down

left arrow - move window left

right arrow - move window right

home - move window to centre of screen

end - move window off screen
