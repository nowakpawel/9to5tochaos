#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>

glm::vec3 Camera::getForward() const {
    float yr = glm::radians(yaw);
    float pr = glm::radians(pitch);
    return glm::normalize(glm::vec3(
        std::cos(pr) * std::sin(yr),
        std::sin(pr),
        -std::cos(pr) * std::cos(yr)
    ));
}

glm::vec3 Camera::getRight() const {
    float yr = glm::radians(yaw);
    return glm::normalize(glm::vec3(std::cos(yr), 0.0f, std::sin(yr)));
}

glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 fwd = getForward();
    return glm::lookAt(position, position + fwd, glm::vec3(0, 1, 0));
}

void Camera::rotate(float dyaw, float dpitch) {
    yaw += dyaw;
    pitch += dpitch;
    pitch = std::clamp(pitch, -89.0f, 89.0f);
    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < -360.0f) yaw += 360.0f;
}
