#pragma once
#include <glm/glm.hpp>
#include "render/Camera.h"

class Player {
public:
    Player();
    void update(float dt, const class World& world);
    void handleMouseMove(float dx, float dy);
    void setMoveInput(float forward, float right, bool jump);

    const Camera& getCamera() const { return m_camera; }
    Camera& getCamera() { return m_camera; }
    glm::vec3 getPosition() const { return m_camera.position; }
    glm::vec3 getForward() const { return m_camera.getForward(); }

    float interactRange{2.5f};
    float dialogRange{2.0f};
    float mouseSensitivity{0.15f};
    float moveSpeed{5.0f};

private:
    Camera m_camera;
    float m_moveForward{0}, m_moveRight{0};
    float m_bobTimer{0};
    float m_bobAmp{0.06f};
    float m_bobFreq{8.0f};
    float m_baseEyeHeight{1.7f};
    bool m_isMoving{false};
};
