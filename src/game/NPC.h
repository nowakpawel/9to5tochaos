#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

enum class NPCType { GRAZYNA_HR, KAMIL_DEV, TOMASZ_MANAGER };
enum class NPCState { PATROL, IDLE, SUSPICIOUS, TALKING };

struct PatrolPoint {
    glm::vec3 pos;
    float waitTime;
};

class NPC {
public:
    NPCType type;
    std::string name;
    glm::vec3 position;
    glm::vec3 color;
    float fovAngle{90.0f};
    float visionRange{8.0f};
    float speed{1.5f};
    NPCState state{NPCState::PATROL};

    std::vector<PatrolPoint> patrolPoints;
    int currentPatrol{0};
    float waitTimer{0};
    float heading{0};

    NPC(NPCType t, const std::string& n, const glm::vec3& startPos, const glm::vec3& col);
    void update(float dt, const glm::vec3& playerPos, bool playerSabotaging);
    void render(class Renderer& r) const;

    bool canSeePlayer(const glm::vec3& playerPos, bool playerSabotaging) const;
    bool isPlayerInFOV(const glm::vec3& playerPos) const;
    float distanceTo(const glm::vec3& pos) const;

    void startTalking();
    void stopTalking();

    std::string dialogId;
};
