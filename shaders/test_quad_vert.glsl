#version 100
// Blinn_Phong_vert.glsl
attribute vec4 position_uv;

varying vec2 oUV;

void main(void)
{
    gl_Position = vec4(position_uv.x, position_uv.y, 0.0, 1.0);

    oUV = position_uv.zw;
    // Transform the position to clip coordinates
}
 
