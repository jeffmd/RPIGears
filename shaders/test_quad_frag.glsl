#version 100
// blinn_phong_frag.glsl


uniform sampler2D DiffuseMap;
varying vec2 oUV;

void main(void)
{
    gl_FragColor = texture2D(DiffuseMap, oUV);
}
