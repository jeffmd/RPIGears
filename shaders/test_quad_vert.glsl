#version 100
// test_quad_vert.glsl

attribute vec4 position_uv;

uniform float scale;

varying vec2 oUV;


void main(void)
{
    gl_Position = vec4(position_uv.xy * scale, 0.0, 1.0);

    oUV = position_uv.zw;
}
 
