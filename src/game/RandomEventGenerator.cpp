#include "RandomEventGenerator.h"
#include <random>
#include <array>
#include <algorithm>

DailyBriefing RandomEventGenerator::generate(int dayNumber, const std::string& prevDayEvent) const {
    DailyBriefing b;
    b.dayNumber = dayNumber;

    std::mt19937 rng(static_cast<unsigned>(dayNumber * 12345));

    static const std::array<TaskType, 3> pool = {
        TaskType::RESTART_BUILD_SERVER,
        TaskType::FIX_PRINTER_JAM,
        TaskType::CLOSE_JIRA_TICKETS
    };

    // Pick 2 distinct tasks
    std::array<int,3> indices = {0,1,2};
    std::shuffle(indices.begin(), indices.end(), rng);
    b.workTasks.push_back(pool[indices[0]]);
    b.workTasks.push_back(pool[indices[1]]);

    static const std::array<std::string, 3> suggestions = {
        "Moze drukarka potrzebuje pomocy?",
        "Serwer wyglada podatnie...",
        "Terminal Kamila jest bez opieki."
    };
    std::uniform_int_distribution<int> sugDist(0, 2);
    b.sabotageSuggestion = suggestions[sugDist(rng)];

    if (prevDayEvent == "printer_jam") {
        b.specialEvent = "KRYZYS: Drukarka calkowicie zepsuta - dzial IT wzywa technika!";
    } else if (prevDayEvent == "critical_bug") {
        b.specialEvent = "KRYZYS: Blad produkcyjny - klienci skarza sie na awarie systemu!";
    } else if (prevDayEvent == "ad_lockout") {
        b.specialEvent = "KRYZYS: Wszyscy pracownicy zablokowania w AD - chaos w biurze!";
    } else {
        b.specialEvent = "";
    }

    return b;
}
