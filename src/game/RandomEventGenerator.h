#pragma once
#include <vector>
#include <string>
#include "TaskSystem.h"

struct DailyBriefing {
    std::vector<TaskType> workTasks;
    std::string sabotageSuggestion;
    std::string specialEvent;
    int dayNumber;
};

class RandomEventGenerator {
public:
    DailyBriefing generate(int dayNumber, const std::string& prevDayEvent = "") const;
};
