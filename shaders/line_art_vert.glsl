#version 100
// line_art_vert.glsl

attribute vec2 position;

uniform vec4 ProjMat;

void main(void)
{
    gl_Position = vec4(position * ProjMat.xy + ProjMat.zw, 0.1, 1.0);
}
 
