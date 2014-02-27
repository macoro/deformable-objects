#version 420 core

in block
{
	vec2 TCoord;
} In;

layout(binding = 0) uniform sampler2D TexFloor;
layout( location = 0 ) out vec4 FragColor;

void main() 
{
	FragColor =  texture( TexFloor, In.TCoord );
}



