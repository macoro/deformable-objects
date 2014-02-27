#version 420 core
#include "common.glsl"
#line 5

out gl_PerVertex
{
        vec4 gl_Position;
};

layout(binding = TRANSFORM0) uniform transform
{
    mat4 ModelViewMatrix;
    mat3 NormalMatrix;
    mat4 ProjectionMatrix;
    mat4 MVP;
} Transform;


layout (location = POSITION) in vec3 VertexPosition;
layout (location = NORMAL) in vec3 VertexNormal;

out block{
    vec3 Position;
    vec3 Normal;
}Out;

void main()
{
    Out.Normal = normalize( Transform.NormalMatrix * VertexNormal);
    Out.Position = vec3( Transform.ModelViewMatrix * vec4(VertexPosition,1.0) );
    gl_Position = MVP * vec4(VertexPosition,1.0);
}
