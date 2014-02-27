#version 420 core
#define IMAG_POSITION 1
#pragma debug(on)

layout (location = 0) in vec3 vVertex;

layout(binding = IMAG_POSITION, rgba32f) uniform image2D Iposition;
layout(location = 0)out vec3 position;

uniform mat4 ViewProjection;
uniform mat4 Model;
uniform int row_size;

out gl_PerVertex
{
        vec4 gl_Position;       
};

void render() {
        position = imageLoad(Iposition, ivec2(gl_VertexID/row_size, gl_VertexID%row_size)).rgb + vec3(Model*vec4(vVertex,1.0));
        //to render
        gl_Position = ViewProjection*vec4(position,1.0);
}
void main()
{
    render();
}
