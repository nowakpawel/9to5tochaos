#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position{0, 1.7f, 0};
    float yaw{0}, pitch{0};
    float fov{90.0f};
    glm::mat4 getViewMatrix() const;
    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
    void rotate(float dyaw, float dpitch);
};
