#ifndef TETRAMESH_H
#define TETRAMESH_H

#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class TetraMesh
{
public:
    TetraMesh(const char *nodefile, const char *elefile, float scale = 1.0f);
    ~TetraMesh();
    //mass
    float getTotalMass();
    void setVolumeMass(float mass);
    //transformation
    void rotate(float angle, glm::vec3 axis);
    void translate(glm::vec3 translation);

    //vertixes, tetrahedrals and edges
    size_t nnode, ntetra, nedge;
    float *m_verts, *m_imass, *m_force, *m_rvolum;
    float *m_velocity, *m_position, *m_prevpos;
    uint *m_indexs, *m_edges;
    glm::mat4 m_transform, m_invtransform;
    int m_rowSize,m_frowSize;

private:
     inline float	VolumeOf(	const glm::vec3& x0, const glm::vec3& x1,
                        const glm::vec3& x2, const glm::vec3& x3)
     {
         return ( glm::dot( (x1-x0),glm::cross( (x2-x0),(x3-x0) ) ) );
     }

     //find edges
     void sort4(uint* a); //sort 4 uint
     int check_index(std::vector<uint>& v, uint index, int i, int f); //v is a sorted vector, return -1 if index already exists;
     void add_edges(std::vector<uint>& v, uint *edges, uint count = 1);
     void findEdges();

     //nodes and elements buffer
     char * elebuffer;
     char * nodebuffer;

     void readElements(const char* elefile);
     void readVertexs(const char* nodefile);
     int nextLine(const char* buffer);

     //mass
     void setTotalMass(float mass);

};

#endif // TETRAMESH_H
