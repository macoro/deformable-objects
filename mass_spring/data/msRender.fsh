#version 420 core
#pragma debug(on)

layout (location = 0)in vec3 Position;
layout (location = 1)in vec3 Normal;

uniform vec4 LightPosition;
uniform vec4 LightIntensity;

uniform vec4 Kd;	// Diffuse reflectivity 
uniform vec4 Ka;	// Ambient reflectivity
uniform vec4 Ks;	// Specular reflectivity
uniform float Shininess;  // Specular shininess factor

layout( location = 0, index = 0) out vec4 FragColor;

vec4 ads( )
{
	vec3 n = Normal;
	vec3 s = normalize( vec3(LightPosition) - Position );
	vec3 v = normalize(vec3(-Position));
	vec3 r = reflect( -s, n );
	return LightIntensity * ( Ka +
		Kd * max( dot(s, n), 0.0 ) +
		Ks * pow( max( dot(r,v), 0.0 ), Shininess ) );
}

void main() 
{
        FragColor = ads();
}

