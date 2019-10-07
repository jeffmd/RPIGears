#version 100
// test_quad_frag.glsl


uniform sampler2D DiffuseMap;
uniform float alimit;
varying vec2 oUV;

void main(void)
{
    gl_FragColor = texture2D(DiffuseMap, oUV).rgbr;
    if (gl_FragColor.a < alimit)
      discard;
}
