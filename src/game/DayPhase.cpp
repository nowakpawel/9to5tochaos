#include "DayPhase.h"
#include <cstdio>

void DayPhase::update(float dt) {
    // 1 real second = 4 game minutes = 4/60 game hours
    gameTimeHours += dt * (GAME_MINUTES_PER_REAL_SECOND / 60.0f);

    if (current == PhaseType::WORK && gameTimeHours >= 12.0f) {
        current = PhaseType::SABOTAGE;
    } else if (current == PhaseType::SABOTAGE && gameTimeHours >= 17.0f) {
        current = PhaseType::END_OF_DAY;
    }
}

void DayPhase::advanceToNextDay() {
    dayNumber++;
    gameTimeHours = 8.0f;
    realTimeAccum = 0.0f;
    current = PhaseType::WORK;
}

std::string DayPhase::getPhaseNamePolish() const {
    switch (current) {
        case PhaseType::WORK:       return "PRACA";
        case PhaseType::SABOTAGE:   return "SABOTAZ";
        case PhaseType::END_OF_DAY: return "KONIEC";
        default:                    return "PRACA";
    }
}

std::string DayPhase::getTimeString() const {
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", getHour(), getMinute());
    return std::string(buf);
}
