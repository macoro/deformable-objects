#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 lookat){
    eye = position;
    look = lookat;
    up = glm::vec3(0.0, 1.0, 0.0);
    projection = glm::perspective(60.0f, 16.0f/9.0f , 0.1f, 1000.0f);

    n = glm::normalize(eye - look);
    u = glm::normalize(glm::cross(up,n));
    v = glm::cross(n,u);

    view = glm::mat4(1.0f);
    updateViewMatrix();
}

Camera::Camera(glm::vec3 position, glm::vec3 lookat, glm::vec3 upv, glm::mat4 pmatrix){
    eye = position;
    look = lookat;
    up = upv;
    projection = pmatrix;

    n = glm::normalize(eye - look);
    u = glm::normalize(glm::cross(up,n));
    v = glm::cross(n,u);

    view = glm::mat4(1.0f);
    updateViewMatrix();
}

Camera::~Camera(){

}

void Camera::slide(float du, float dv, float dn){
    eye.x += du * u.x + dv * v.x + dn * n.x;
    eye.y += du * u.y + dv * v.y + dn * n.y;
    eye.z += du * u.z + dv * v.z + dn * n.z;
    //update viewmatrix
    float *m = glm::value_ptr(view);
    m[12] = glm::dot(-eye, u);
    m[13] = glm::dot(-eye,v);
    m[14] = glm::dot(-eye,n);

}

void Camera::rotAxes(glm::vec3 &a, glm::vec3 &b, float angle){
    // rotate orthogonal vectors a (like x axis) and b(like y axis) through angle degrees
    float ang = 3.14159265/180*angle;
    float C = cos(ang), S = sin(ang);
    glm::vec3 t(C * a.x - S * b.x, C * a.y - S * b.y, C * a.z - S * b.z);
    b = glm::vec3(S * a.x + C * b.x, S * a.y + C * b.y, S * a.z + C * b.z);
    a = t;
}

void Camera::roll(float angle){
    rotAxes( u, v, angle );
    //update viewmatrix
    float *m = glm::value_ptr(view);
    m[0] = u.x; m[4] = u.y; m[8] = u.z; m[12] = glm::dot(-eye, u);
    m[1] = v.x; m[5] = v.y; m[9] = v.z; m[13] = glm::dot(-eye,v);
}

void Camera::pitch(float angle){
    rotAxes( n, v, angle );
    //update ViewMatrix
    float *m = glm::value_ptr(view);
    m[1] = v.x; m[5] = v.y; m[9] = v.z; m[13] = glm::dot(-eye,v);
    m[2] = n.x; m[6] = n.y; m[10] = n.z; m[14] = glm::dot(-eye,n);

}

void Camera::yaw(float angle){
    rotAxes(u, n, angle);
    //update ViewMatrix
    float *m = glm::value_ptr(view);
    m[0] = u.x; m[4] = u.y; m[8] = u.z; m[12] = glm::dot(-eye, u);
    m[2] = n.x; m[6] = n.y; m[10] = n.z; m[14] = glm::dot(-eye,n);
}

void Camera::updateViewMatrix(){
    float *m = glm::value_ptr(view);
    m[0] = u.x; m[4] = u.y; m[8] = u.z; m[12] = glm::dot(-eye, u);
    m[1] = v.x; m[5] = v.y; m[9] = v.z; m[13] = glm::dot(-eye,v);
    m[2] = n.x; m[6] = n.y; m[10] = n.z; m[14] = glm::dot(-eye,n);
}

glm::mat4 &Camera::ViewMatrix(){
    return view;
}
