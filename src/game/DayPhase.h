#pragma once
#include <string>

enum class PhaseType { WORK, SABOTAGE, END_OF_DAY };

class DayPhase {
public:
    PhaseType current{PhaseType::WORK};
    int dayNumber{1};

    float gameTimeHours{8.0f};
    float realTimeAccum{0};
    static constexpr float GAME_MINUTES_PER_REAL_SECOND = 4.0f;

    void update(float dt);
    void advanceToNextDay();
    bool isWorkPhase()    const { return current == PhaseType::WORK; }
    bool isSabotagePhase() const { return current == PhaseType::SABOTAGE; }
    bool isEndOfDay()     const { return current == PhaseType::END_OF_DAY; }

    int getHour()   const { return static_cast<int>(gameTimeHours); }
    int getMinute() const { return static_cast<int>((gameTimeHours - getHour()) * 60.0f); }

    std::string getPhaseNamePolish() const;
    std::string getTimeString() const;
};
