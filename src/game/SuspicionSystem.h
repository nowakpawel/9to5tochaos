#pragma once
#include <array>
#include <string>

enum class SuspicionGroup { BOSS=0, TEAM=1, SECURITY=2 };

struct SuspicionSystem {
    std::array<float, 3> levels{0,0,0};
    float decayRate{0.5f};

    void addSuspicion(SuspicionGroup group, float delta);
    void update(float dt, bool nearNPC);
    bool isCaught() const;
    SuspicionGroup getCaughtGroup() const;
    float get(SuspicionGroup g) const { return levels[static_cast<int>(g)]; }
    void reset() { levels = {0,0,0}; }

    static const char* groupName(SuspicionGroup g);
};
