#version 100
// test_quad_vert.glsl

attribute vec4 position_uv;

uniform vec4 ProjMat;

varying vec2 oUV;


void main(void)
{
    gl_Position = vec4(position_uv.xy * ProjMat.xy + ProjMat.zw, 0.1, 1.0);
    oUV = position_uv.zw;
}
 
