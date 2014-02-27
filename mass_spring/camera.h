// camera class
#ifndef _GECAMERA
#define _GECAMERA

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{

private:
    glm::vec3 eye, look, up, u, v, n;
    glm::mat4 projection, view;

    void rotAxes(glm::vec3& a, glm::vec3& b, float angle);
    void updateViewMatrix();

public:
    Camera(glm::vec3 position, glm::vec3 lookat, glm::vec3 upv, glm::mat4 pmatrix);
    Camera(glm::vec3 position, glm::vec3 lookat);
    ~Camera();

    inline glm::vec3 getposition() { return eye; }
    inline void setprojection(glm::mat4& pmatrix) { projection = pmatrix; }
    inline glm::mat4 getprojection() { return projection; }

	void roll(float angle);
	void pitch(float angle);
	void yaw(float angle);
    void slide(float du, float dv, float dn);
    glm::mat4& ViewMatrix();
};


#endif
