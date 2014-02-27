#version 430 core
#extension GL_NV_shader_atomic_float : require


#define IMAG_FORCE 0
#define IMAG_POSITION 1

layout(lines) in;
layout( points, max_vertices=200 ) out;

layout(binding = IMAG_FORCE, r32f) uniform image2D Iforce;
layout(binding = IMAG_POSITION, rgba32f) coherent uniform image2D Iposition;

uniform float K;
uniform float damp;
uniform int frow_size;
uniform mat4 Model;

in gl_PerVertex
{
        vec4  gl_Position;
} gl_in[];

out gl_PerVertex
{
        vec4 gl_Position;
};

in block
{
    vec3 initposition;
    vec3 distance;
    vec3 velocity;
    float mass;
    int row;
    int col;
    int ID;
}  In[];

//functions to fight vs numerical errors
bool KnuthCompare(float a, float b, float relError)
{
    return ( abs(a-b) <= relError*max(abs(a),abs(b)) );
}

bool IsZero(float x,float ep)
{
    return (abs(x)<ep);
}

float trunc6(float f)
{
    return floor(f*1000000.0f + 0.5f)/1000000.0f;
}

//Hookes Law for Damped Spring
const float ld = 1.25f; const float sd = 0.75f;
vec3 get_dsforce()
{
    vec3 initposition_i = vec3(Model*vec4(In[0].initposition,1.0));
    vec3 initposition_j = vec3(Model*vec4(In[1].initposition,1.0));
    float lrij = length(initposition_j - initposition_i);
    vec3 position_i = In[0].distance + initposition_i;
    vec3 position_j = In[1].distance + initposition_j;
    vec3 pij = position_j - position_i;
    float lpij = length(pij);

    vec3 rvel = In[1].velocity - In[0].velocity;
    rvel.x = (KnuthCompare(In[1].velocity.x + 1.0f, In[0].velocity.x + 1.0f, 1.0e-5f))? 0.0f : rvel.x;
    rvel.y = (KnuthCompare(In[1].velocity.y + 1.0f, In[0].velocity.y + 1.0f, 1.0e-5f))? 0.0f : rvel.y;
    rvel.z = (KnuthCompare(In[1].velocity.z + 1.0f, In[0].velocity.z + 1.0f, 1.0e-5f))? 0.0f : rvel.z;

    //Strain Limiting
    if( lpij > ld*lrij )
    {
        vec3 x0 = In[0].distance + (In[1].mass/(In[0].mass+In[1].mass))*(lpij - ld*lrij)*(pij/lpij);
        vec3 x1 = In[1].distance - (In[0].mass/(In[0].mass+In[1].mass))*(lpij - ld*lrij)*(pij/lpij);

        //imageStore(Iposition, ivec2(In[0].row, In[0].col), vec4(x0, 1.0f));
        //imageStore(Iposition, ivec2(In[1].row, In[1].col), vec4(x1, 1.0f));

        pij = x1 - x0 + vec3(Model*vec4(In[1].initposition,1.0)) - vec3(Model*vec4(In[0].initposition,1.0));
        lpij = length(pij);
    }

    else if( lpij < sd*lrij )
    {
        vec3 x0 = In[0].distance + (In[1].mass/(In[0].mass+In[1].mass))*(lpij - sd*lrij)*(pij/lpij);
        vec3 x1 = In[1].distance - (In[0].mass/(In[0].mass+In[1].mass))*(lpij - sd*lrij)*(pij/lpij);

        //imageStore(Iposition, ivec2(In[0].row, In[0].col), vec4(x0, 1.0f));
        //imageStore(Iposition, ivec2(In[1].row, In[1].col), vec4(x1, 1.0f));

        pij = x1 - x0 + vec3(Model*vec4(In[1].initposition,1.0)) - vec3(Model*vec4(In[0].initposition,1.0));
        lpij = length(pij);
    }

    float rdist = ( lpij - lrij );
    float tol = length(In[1].distance - In[0].distance);
    rdist = (IsZero(tol, 1.0e-6f))? 0.0f : rdist;

    int ind = (rdist > 0)? 1 : 0;
    vec3 fij =  (K*rdist/(lrij + 1.0f) + ind*damp*dot(rvel,pij)/lpij )*(pij/lpij);
    return fij;
}

void main(){
    vec3 fij = get_dsforce();
    //save force
    imageAtomicAdd(Iforce, ivec2( (3*In[0].ID)/frow_size,  (3*In[0].ID)%frow_size)  , trunc6(fij.x));
    imageAtomicAdd(Iforce, ivec2( (3*In[0].ID+1)/frow_size,(3*In[0].ID+1)%frow_size), trunc6(fij.y));
    imageAtomicAdd(Iforce, ivec2( (3*In[0].ID+2)/frow_size,(3*In[0].ID+2)%frow_size), trunc6(fij.z));

    imageAtomicAdd(Iforce, ivec2( (3*In[1].ID)/frow_size,  (3*In[1].ID)%frow_size)  , trunc6(-fij.x));
    imageAtomicAdd(Iforce, ivec2( (3*In[1].ID+1)/frow_size,(3*In[1].ID+1)%frow_size), trunc6(-fij.y));
    imageAtomicAdd(Iforce, ivec2( (3*In[1].ID+2)/frow_size,(3*In[1].ID+2)%frow_size), trunc6(-fij.z));
}
