#version 100
// blinn_phong_frag.glsl


uniform vec4 MaterialColor;
uniform sampler2D DiffuseMap;
uniform float _gl_InstanceID;
//#define UBO_M4(index) mat4(UBO[index], UBO[index+1], UBO[index+2], UBO[index+3])
//#define MaterialColor UBO[14]

varying vec3 N;
//varying vec3 V;
varying vec3 H;
varying vec3 L;
varying vec2 oUV;

void main(void)
{
    //discard;
    //vec3 l = normalize(L + V);
    //vec3 N = normalize(N);

    vec4 diffuse = vec4(0.8, 0.7, 0.55, 1.0) * max(dot(L, N), 0.0);
    // get bump map vector, again expand from range-compressed
    vec4 diffCol = texture2D(DiffuseMap, oUV);
    diffCol.r += _gl_InstanceID/10.0;
    // modulate diffuseMap with base material color
    //float depth = (1.0 - gl_FragCoord.z)*40.0;
    gl_FragColor = MaterialColor * diffCol * (vec4(0.15, 0.5, 0.7, 1.0) +  diffuse);
    //gl_FragColor = vec4(depth, depth, depth, 1.0) ;
    //   add  specular
    // materials that have more red in them are shinnier
    //vec3 H = reflect(-l, N);//normalize(l + normalize(V));
    float ndoth = max(dot(N, H), 0.0);
    //float grey = dot( vec3(0.21, 0.72, 0.07), diffCol.rgb );
    gl_FragColor.rgb += vec3(0.8, 0.9, 0.6) * pow(ndoth, 16.0 );
}
