#include "NPC.h"
#include "render/Renderer.h"
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>

NPC::NPC(NPCType t, const std::string& n, const glm::vec3& startPos, const glm::vec3& col)
    : type(t), name(n), position(startPos), color(col) {
    switch (t) {
        case NPCType::GRAZYNA_HR:     dialogId = "grazyna_suspicious_start"; break;
        case NPCType::KAMIL_DEV:      dialogId = "kamil_printer_start"; break;
        case NPCType::TOMASZ_MANAGER: dialogId = "tomasz_manager_start"; break;
        default: dialogId = "kamil_printer_start"; break;
    }
}

void NPC::update(float dt, const glm::vec3& playerPos, bool playerSabotaging) {
    if (state == NPCState::TALKING) return;

    if (canSeePlayer(playerPos, playerSabotaging) && playerSabotaging) {
        state = NPCState::SUSPICIOUS;
    }

    if (patrolPoints.empty()) return;

    PatrolPoint& target = patrolPoints[currentPatrol];
    glm::vec3 toTarget = target.pos - position;
    toTarget.y = 0;
    float dist = glm::length(toTarget);

    if (dist < 0.2f) {
        // Wait at patrol point
        if (waitTimer < target.waitTime) {
            waitTimer += dt;
        } else {
            waitTimer = 0;
            currentPatrol = (currentPatrol + 1) % (int)patrolPoints.size();
        }
    } else {
        glm::vec3 dir = glm::normalize(toTarget);
        position += dir * speed * dt;
        heading = std::atan2(dir.x, dir.z);
    }
}

bool NPC::isPlayerInFOV(const glm::vec3& playerPos) const {
    glm::vec3 toPlayer = playerPos - position;
    toPlayer.y = 0;
    if (glm::length(toPlayer) < 0.001f) return true;
    toPlayer = glm::normalize(toPlayer);

    glm::vec3 npcFwd{std::sin(heading), 0, std::cos(heading)};
    float cosAngle = glm::dot(toPlayer, npcFwd);
    float halfAngle = glm::radians(fovAngle * 0.5f);
    return cosAngle >= std::cos(halfAngle);
}

bool NPC::canSeePlayer(const glm::vec3& playerPos, bool playerSabotaging) const {
    float d = distanceTo(playerPos);
    if (playerSabotaging && d < 3.0f) return true;
    return d <= visionRange && isPlayerInFOV(playerPos);
}

float NPC::distanceTo(const glm::vec3& pos) const {
    return glm::length(position - pos);
}

void NPC::startTalking() {
    state = NPCState::TALKING;
}

void NPC::stopTalking() {
    state = NPCState::PATROL;
}

void NPC::render(Renderer& r) const {
    // Body
    glm::vec3 mn{position.x - 0.3f, position.y,        position.z - 0.3f};
    glm::vec3 mx{position.x + 0.3f, position.y + 1.8f, position.z + 0.3f};
    r.drawBox3D(mn, mx, color);

    // Head (slightly lighter)
    glm::vec3 headColor{
        std::min(1.0f, color.r * 1.3f),
        std::min(1.0f, color.g * 1.3f),
        std::min(1.0f, color.b * 1.3f)
    };
    glm::vec3 hmn{position.x - 0.2f, position.y + 1.8f, position.z - 0.2f};
    glm::vec3 hmx{position.x + 0.2f, position.y + 2.1f, position.z + 0.2f};
    r.drawBox3D(hmn, hmx, headColor);
}
