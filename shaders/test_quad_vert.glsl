#version 100
// test_quad_vert.glsl
attribute vec4 position_uv;

varying vec2 oUV;

#define SCALE 0.12

void main(void)
{
    gl_Position = vec4(position_uv.x * SCALE, position_uv.y * SCALE, 0.0, 1.0);

    oUV = position_uv.zw;
    // Transform the position to clip coordinates
}
 
