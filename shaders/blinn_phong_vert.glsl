#version 100
// Blinn_Phong_vert.glsl
attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

uniform vec4 UBO[9];

varying vec3 N;
//varying vec3 V;
varying vec3 H;
varying vec3 L;
varying vec2 oUV;

#define UBO_M4(index) mat4(UBO[index], UBO[index+1], UBO[index+2], UBO[index+3])
#define ModelViewMatrix UBO_M4(0)
#define LightPosition UBO[4]
#define CameraProjectionMatrix UBO_M4(5)

void main(void)
{
    vec4 pos = vec4(position, 1.0);
   // None of the vectors are normalized until in the fragment shader
// Calculate the normal vector for this vertex, in view space (
    N = normalize(ModelViewMatrix * vec4(normal, 0.0)).xyz;
    // Calculate the view vector
    pos = ModelViewMatrix * pos;
    vec3 V = -pos.xyz;
    // Calculate the light vector for this vertex
    L = normalize(LightPosition.xyz + V);
//    L = LightPosition.xyz;
    // calculate half angle
    H = normalize(L + normalize(V));

    oUV = uv;
    // Transform the position to clip coordinates
    gl_Position = CameraProjectionMatrix * pos;
    gl_PointSize = 2.0;
}
 
