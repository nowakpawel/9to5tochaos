#include "SuspicionSystem.h"
#include <algorithm>

void SuspicionSystem::addSuspicion(SuspicionGroup group, float delta) {
    int i = static_cast<int>(group);
    levels[i] = std::clamp(levels[i] + delta, 0.0f, 100.0f);
}

void SuspicionSystem::update(float dt, bool nearNPC) {
    if (!nearNPC) {
        for (auto& lv : levels) {
            lv = std::max(0.0f, lv - decayRate * dt);
        }
    }
}

bool SuspicionSystem::isCaught() const {
    for (float lv : levels) {
        if (lv >= 100.0f) return true;
    }
    return false;
}

SuspicionGroup SuspicionSystem::getCaughtGroup() const {
    for (int i = 0; i < 3; ++i) {
        if (levels[i] >= 100.0f) return static_cast<SuspicionGroup>(i);
    }
    return SuspicionGroup::BOSS;
}

const char* SuspicionSystem::groupName(SuspicionGroup g) {
    switch (g) {
        case SuspicionGroup::BOSS:     return "BOSS";
        case SuspicionGroup::TEAM:     return "TEAM";
        case SuspicionGroup::SECURITY: return "SECURITY";
        default: return "UNKNOWN";
    }
}
