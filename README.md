RPIGears
========

OpengGL ES port of the classic gl gears demo for the Raspberry Pi written in c.

Building
--------
The following files should be in the build directory:

* LICENSE
* Makefile
* README.md

The c source files are in the src directory, c header files are in the include directory.
GLSL vertex and fragment shader source files are located in the shaders directory. 


In a terminal session, change directory to the build directory where the 
RPIGears Makefile file is located.  Use make at the commandline to build
RPIGears:

`make`

No special libraries/packages are required to build if using Raspbian.


Running
-------

To run the demo type `./RPIGears`  The demo runs in a X11 window.  To
exit the demo press esc or enter key on the keyboard.


Command line Options
--------------------
usage: ./RPIGears.bin [options]

options: -vsync | -exit | -info | -vbo


-vsync : wait for vertical sync before new frame is displayed

-exit  : automatically exit RPIGears after 30 seconds

-info  : display opengl driver info

-vbo   : use vertex buffer object in GPU memory

-line  : draw lines only, wire frame mode

-nospin: gears don't turn


Options can be used in any combination.


Speed keys while demo is running
-----------------------------
i - print GL info to console

h - print help info

l - toggle draw mode GL_TRIAGLES/GL_LINES/GL_POINTS

o - print command line options to console

v - toggle vertical sync on/off

< - decrease gear spin rate

> - increase gear spin rate

p - stop the gears from rotating

a - move camera left

d - move camera right

w - move camera up

s - move camera down

r - move camera away from gears

f - move camera toward gears

I - add another draw instance of the gears

O - remove an instance of the gears

R - reload shaders

b - toggle use of vertex buffer objects for gear vertex data
