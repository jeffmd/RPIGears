#version 100
// test_quad_frag.glsl


uniform sampler2D DiffuseMap;
varying vec2 oUV;

void main(void)
{
    gl_FragColor = texture2D(DiffuseMap, oUV).rgbr;
    if (gl_FragColor.a == 0.0)
      discard;
}
