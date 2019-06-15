#version 100
// test_quad_vert.glsl

attribute vec4 position_uv;

uniform float scale;
uniform mat4 ProjMat;

varying vec2 oUV;


void main(void)
{
    gl_Position = ProjMat * vec4(position_uv.xy * scale, 0.1, 1.0);
    oUV = position_uv.zw;
}
 
