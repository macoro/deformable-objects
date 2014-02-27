#ifndef TRIMESH_H
#define TRIMESH_H

#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace program
{
    enum type
    {
        VERTEX,
        FRAGMENT,
        MAX
    };
}//namespace program

namespace buffer
{
    enum type
    {
        VERTEX,
        ELEMENT,
        TRANSFORM,
        LIGHTMATERIAL,
        MAX
    };
}//namespace buffer

class TriMesh
{
public:
    TriMesh(float *vertexs, float *normals, uint *faces);
    float *vertexs, *normals, *texcoords;
    uint *faces;

    bool begin();
    void renderGL();
    bool end();
    //transformation
    void rotate(float angle, glm::vec3 axis);
    void translate(glm::vec3 translation);
    void scale(glm::vec3 factors);
private:
    glm::mat4 m_transform, m_invtransform;
    bool initProgram();
    bool initBuffer();
    bool initVertexArray();
    bool initDebugOutput();


    static std::string const SHADER_VERT_SOURCE;
    static std::string const SHADER_FRAG_SOURCE;

    GLuint PipelineName;
    GLuint ProgramName;
    GLuint VertexArrayName;
    GLuint BufferName[buffer::MAX];

};

#endif // TRIMESH_H
