#include "Player.h"
#include "World.h"
#include <cmath>

Player::Player() {
    m_camera.position = {10.0f, m_baseEyeHeight, 6.0f};
}

void Player::setMoveInput(float forward, float right, bool /*jump*/) {
    m_moveForward = forward;
    m_moveRight   = right;
}

void Player::handleMouseMove(float dx, float dy) {
    m_camera.rotate(dx * mouseSensitivity, -dy * mouseSensitivity);
}

void Player::update(float dt, const World& world) {
    glm::vec3 fwd = m_camera.getForward();
    glm::vec3 right = m_camera.getRight();

    // Flatten to XZ plane
    fwd.y = 0;
    if (glm::length(fwd) > 0.001f) fwd = glm::normalize(fwd);
    right.y = 0;
    if (glm::length(right) > 0.001f) right = glm::normalize(right);

    glm::vec3 velocity = (fwd * m_moveForward + right * m_moveRight);
    m_isMoving = glm::length(velocity) > 0.001f;
    if (m_isMoving) velocity = glm::normalize(velocity) * moveSpeed;

    glm::vec3 newPos = m_camera.position;
    newPos.x += velocity.x * dt;
    newPos.z += velocity.z * dt;
    newPos = world.resolveCollision(newPos, 0.3f);

    // Head bobbing
    if (m_isMoving) {
        m_bobTimer += dt;
        newPos.y = m_baseEyeHeight + std::sin(m_bobTimer * m_bobFreq) * m_bobAmp;
    } else {
        m_bobTimer = 0.0f;
        newPos.y = m_baseEyeHeight;
    }

    m_camera.position = newPos;
}
