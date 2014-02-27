#version 420 core

uniform mat4 MVP;

out gl_PerVertex
{
 	vec4 gl_Position;    
};

out block
{
	vec2 TCoord;
} Out;

const int VertexCount = 4;
const vec3 Position[VertexCount] = vec3[]( vec3(-10.0f, -1.0f, 10.0f),
                                        vec3( 10.0f, -1.0f, 10.0f),
                                        vec3( 10.0f, -1.0f,-10.0f),
                                        vec3(-10.0f, -1.0f,-10.0f) );
	
const vec2 TextCoord[VertexCount] = vec2[](vec2(0.0f,1.0f), vec2(1.0f,1.0f),
                                        vec2(1.0f,0.0f), vec2(0.0f,0.0f) );

float scale = 3.0;

void main()
{
        Out.TCoord = TextCoord[gl_VertexID];
	gl_Position = MVP*vec4(scale*Position[gl_VertexID], 1.0);
}

