#version 430 core

#define IMAG_POSITION 1
#define IMAG_VELOCITY 3

layout(binding = IMAG_POSITION, rgba32f) uniform image2D Iposition;
layout(binding = IMAG_VELOCITY, rgba32f) uniform image2D Ivelocity;

layout (location = 0) in vec3 vVertex;
layout (location = 1) in float inv_mass;

uniform int row_size;

out gl_PerVertex
{
        vec4 gl_Position;
};

out block
{
    vec3 initposition;
    vec3 distance;
    vec3 velocity;
    float mass;
    int row;
    int col;
    int ID;
} Out;

void main()
{
    Out.mass = inv_mass;
    Out.initposition = vVertex;
    int row = gl_VertexID/row_size;
    int col = gl_VertexID%row_size;
    Out.distance = imageLoad(Iposition, ivec2(row,col)).rgb;
    Out.velocity = imageLoad(Ivelocity, ivec2(row,col)).rgb;
    Out.row = row;
    Out.col = col;
    Out.ID = gl_VertexID;
}
