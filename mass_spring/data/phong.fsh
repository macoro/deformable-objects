#version 420 core

#include "common.glsl"
#line 5

in block{
    vec3 Position;
    vec3 Normal;
}In;

layout(binding = LIGHTMATERIAL) uniform LightMaterial{
    vec4 LightPosition;
    vec4 LightIntensity;
    vec4 Kd;	// Diffuse reflectivity
    vec4 Ka;	// Ambient reflectivity
    vec4 Ks;	// Specular reflectivity
    float Shininess;  // Specular shininess factor
}LM;

layout( location = FRAG_COLOR, index = 0) out vec4 FragColor;

vec4 ads( )
{
    vec3 n = In.Normal;
    vec3 s = normalize( vec3(LM.LightPosition) - In.Position );
    vec3 v = normalize(vec3(-In.Position));
    vec3 r = reflect( -s, n );
    return LM.LightIntensity * ( LM.Ka +
                              LM.Kd * max( dot(s, n), 0.0 ) +
                              LM.Ks * pow( max( dot(r,v), 0.0 ), LM.Shininess ) );
}

void main()
{
        FragColor = ads();
}

