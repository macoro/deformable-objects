#version 430 core
#extension GL_NV_shader_atomic_float : require

#define IMAG_FORCE 0
#define IMAG_POSITION 1
#define IMAG_PREVPOS  2
#define IMAG_VELOCITY 3

layout(binding = IMAG_FORCE, r32f) uniform image2D Iforce;
layout(binding = IMAG_POSITION, rgba32f)coherent uniform image2D Iposition;
layout(binding = IMAG_PREVPOS, rgba32f) uniform image2D Iprevpos;
layout(binding = IMAG_VELOCITY, rgba32f) uniform image2D Ivelocity;

layout (location = 0) in vec3 vVertex;
layout (location = 1) in float inv_mass;

uniform mat4 Model;
uniform int row_size;
uniform int frow_size;
uniform float step;

out gl_PerVertex
{
        vec4 gl_Position;
};

const vec3 gravity = vec3(0.0f, -5.0f ,0.0f);
const float plane = -3.0f;

bool IsZero(float x,float ep)
{
    return (abs(x)<ep);
}

void updateposition(){
        int row = gl_VertexID/row_size;
        int col = gl_VertexID%row_size;

        vec3 f;
        f.x = imageAtomicExchange(Iforce, ivec2((3*gl_VertexID)/frow_size,(3*gl_VertexID)%frow_size), 0.0f);
        f.y = imageAtomicExchange(Iforce, ivec2((3*gl_VertexID+1)/frow_size,(3*gl_VertexID+1)%frow_size), 0.0f);
        f.z = imageAtomicExchange(Iforce, ivec2((3*gl_VertexID+2)/frow_size,(3*gl_VertexID+2)%frow_size), 0.0f);

        vec3 position = imageLoad(Iposition, ivec2(row,col)).rgb;
        vec3 prevpos = imageLoad(Iprevpos, ivec2(row,col)).rgb;

        //verlet integration
        vec3 new_pos = 2.0f*position - prevpos + step*step*(f*inv_mass + gravity);
        vec3 velocity = imageLoad(Ivelocity, ivec2(row,col)).rgb + step*(f*inv_mass + gravity);
        //check collision
        vec3 initposition = vec3(Model*vec4(vVertex, 1.0));
        if( new_pos.y + initposition.y - plane < 0.0f )
        {
            new_pos = vec3(position.x,plane - initposition.y, position.z);
            velocity = vec3(0.0f);
        }
        imageStore(Iprevpos, ivec2(row,col), vec4(position, 1.0));
        imageStore(Iposition, ivec2(row,col), vec4(new_pos, 1.0));
        imageStore(Ivelocity, ivec2(row,col), vec4(velocity, 1.0));
}

void main(){
    updateposition();
}

