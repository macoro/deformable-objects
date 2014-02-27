#version 420 core
#pragma debug(on)

layout(lines_adjacency) in;
layout( triangle_strip, max_vertices=200 ) out;

layout (location = 0) in vec3 position[4];

layout (location = 0)out vec3 gPosition;
layout (location = 1)out vec3 gNormal;

in gl_PerVertex
{
        vec4  gl_Position;
} gl_in[];

out gl_PerVertex 
{
        vec4 gl_Position;
};

uniform mat3 NormalMatrix;
uniform mat4 View;

vec3[4] comp_normal(){
	// 0-1 0-2 0-3 1-2 1-3 2-3
	vec3 edges[6] =vec3[6]( position[1] - position[0], position[2] - position[0],
				position[3] - position[0], position[2] - position[1],
				position[3] - position[1], position[3] - position[2] );
	vec3 normals[4];
	normals[0] = normalize( cross( edges[0], edges[2] ) ) ;
	normals[1] = normalize( cross( edges[5], -edges[3] ) );
	normals[2] = normalize( cross( edges[1], edges[0] ) );
	normals[3] = normalize( cross(-edges[1],  edges[5] ) );
	
	return normals;
}

void produceVertex( int id, vec3 normal){
        gPosition = vec3( View*vec4(position[id],1.0) );
        gNormal = normalize( NormalMatrix*normal);
	gl_Position = gl_in[id].gl_Position;		        
	EmitVertex();
}

void render() {
	vec3 normals[4] = comp_normal();
	// triangle 013
	produceVertex ( 0, normals[0] );
	produceVertex ( 1, normals[0] );
	produceVertex ( 3, normals[0] );
	EndPrimitive();
	// triangle 312
	produceVertex ( 3, normals[1] );
	produceVertex ( 1, normals[1] );
	produceVertex ( 2, normals[1] );
	EndPrimitive();
	// triangle 102
	produceVertex ( 1, normals[2] );
	produceVertex ( 0, normals[2] );
	produceVertex ( 2, normals[2] );
	EndPrimitive();	
	// triangle 203
	produceVertex ( 2, normals[3] );
	produceVertex ( 0, normals[3] );
	produceVertex ( 3, normals[3] );
	EndPrimitive();	
}

void main()
{
        render();
}
