#pragma once
#include <string>

enum class SabotageType { PRINTER_JAM, COMMIT_BUG, AD_LOCKOUT };

struct SabotageAction {
    SabotageType type;
    std::string name;
    std::string description;
    float holdDuration;
    float teamSuspicion;
    float bossSuspicion;
    float securitySuspicion;
    std::string nextDayEvent;

    static SabotageAction create(SabotageType t);
};

struct SabotageProgress {
    bool active{false};
    float progress{0};
    SabotageType type{SabotageType::PRINTER_JAM};
    float holdDuration{0};

    void start(SabotageType t, float duration);
    bool update(float dt, bool holdingKey);
    void cancel();
};
