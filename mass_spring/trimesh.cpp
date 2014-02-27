#include "trimesh.h"


TriMesh::TriMesh(float *vertexs, float *normals, uint *faces)
{
    this->vertexs = vertexs;
    this->normals = normals;
    this->faces = faces;
}

void TriMesh::rotate(float angle, glm::vec3 axis){
    m_transform = glm::rotate(m_transform, angle, axis);
    m_invtransform = glm::inverse(m_transform);
}

void TriMesh::translate(glm::vec3 translation){
    m_transform = glm::translate(m_transform,translation);
    m_invtransform = glm::inverse(m_transform);
}

void TriMesh::scale(glm::vec3 factors){
    m_transform = glm::scale(m_transform, factors);
    m_invtransform = glm::inverse(m_transform);
}

