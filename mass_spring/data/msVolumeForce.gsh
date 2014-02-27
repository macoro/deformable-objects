#version 430 core
#extension GL_NV_shader_atomic_float : require

#define IMAG_FORCE 0

layout(lines_adjacency) in;
layout( points, max_vertices=200 ) out;

layout(binding = IMAG_FORCE, r32f) uniform image2D Iforce;

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
    return floor(f*1000000.0 + 0.5)/1000000.0;
}

//Volume conservation force
vec3[4] get_vcforce(){
    //find rest volume
    vec3 initp0 = vec3(Model*vec4(In[0].initposition,1.0));
    vec3 a=vec3(Model*vec4(In[1].initposition,1.0)) - initp0;
    vec3 b=vec3(Model*vec4(In[2].initposition,1.0)) - initp0;
    vec3 c=vec3(Model*vec4(In[3].initposition,1.0)) - initp0;
    float rvolume = dot(a,cross(b,c));   // 6*V

    vec3 p01 = In[1].distance - In[0].distance + a;
    vec3 p02 = In[2].distance - In[0].distance + b;
    vec3 p03 = In[3].distance - In[0].distance + c;

    float volum = dot(p01, cross(p02, p03));
    float C = volum - rvolume;
    C = (KnuthCompare(volum + 1.0f, rvolume + 1.0f, 1.0e-5f))? 0.0f : C;

    if(IsZero(volum,1.0e-8f)){
        p01 = a;
        p02 = b;
        p03 = c;
    }

    vec3 norm0 = cross( (p02 - p01), (p03 - p01) );
    vec3 norm1 = cross( p03, p02 );
    vec3 norm2 = cross( p01, p03 );
    vec3 norm3 = cross( p02, p01 );

    float len0 = length(norm0);
    float len1 = length(norm1);
    float len2 = length(norm2);
    float len3 = length(norm3);
    //F0
    float F0 = K*C/(rvolume+1);
    int ind = (IsZero(F0, 1.0e-6f))? 0 : 1;

    vec3 vc_force[4];
    vc_force[0] = (F0 - ind*damp*dot((3.0f*In[0].velocity -In[1].velocity -In[2].velocity -In[3].velocity),norm0)/len0)*norm0/len0;
    vc_force[1] = (F0 - ind*damp*dot((3.0f*In[1].velocity -In[0].velocity -In[2].velocity -In[3].velocity),norm1)/len1)*norm1/len1;
    vc_force[2] = (F0 - ind*damp*dot((3.0f*In[2].velocity -In[0].velocity -In[1].velocity -In[3].velocity),norm2)/len2)*norm2/len2;
    vc_force[3] = (F0 - ind*damp*dot((3.0f*In[3].velocity -In[0].velocity -In[1].velocity -In[2].velocity),norm3)/len3)*norm3/len3;

    return vc_force;
}

void main(){
    vec3 vcf[4] = get_vcforce();
    //save force
    imageAtomicAdd(Iforce, ivec2( (3*In[0].ID)/frow_size,  (3*In[0].ID)%frow_size)  , trunc6(vcf[0].x));
    imageAtomicAdd(Iforce, ivec2( (3*In[0].ID+1)/frow_size,(3*In[0].ID+1)%frow_size), trunc6(vcf[0].y));
    imageAtomicAdd(Iforce, ivec2( (3*In[0].ID+2)/frow_size,(3*In[0].ID+2)%frow_size), trunc6(vcf[0].z));

    imageAtomicAdd(Iforce, ivec2( (3*In[1].ID)/frow_size,  (3*In[1].ID)%frow_size)  , trunc6(vcf[1].x));
    imageAtomicAdd(Iforce, ivec2( (3*In[1].ID+1)/frow_size,(3*In[1].ID+1)%frow_size), trunc6(vcf[1].y));
    imageAtomicAdd(Iforce, ivec2( (3*In[1].ID+2)/frow_size,(3*In[1].ID+2)%frow_size), trunc6(vcf[1].z));

    imageAtomicAdd(Iforce, ivec2( (3*In[2].ID)/frow_size,  (3*In[2].ID)%frow_size)  , trunc6(vcf[2].x));
    imageAtomicAdd(Iforce, ivec2( (3*In[2].ID+1)/frow_size,(3*In[2].ID+1)%frow_size), trunc6(vcf[2].y));
    imageAtomicAdd(Iforce, ivec2( (3*In[2].ID+2)/frow_size,(3*In[2].ID+2)%frow_size), trunc6(vcf[2].z));

    imageAtomicAdd(Iforce, ivec2( (3*In[3].ID)/frow_size,  (3*In[3].ID)%frow_size)  , trunc6(vcf[3].x));
    imageAtomicAdd(Iforce, ivec2( (3*In[3].ID+1)/frow_size,(3*In[3].ID+1)%frow_size), trunc6(vcf[3].y));
    imageAtomicAdd(Iforce, ivec2( (3*In[3].ID+2)/frow_size,(3*In[3].ID+2)%frow_size), trunc6(vcf[3].z));
}
