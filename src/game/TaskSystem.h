#pragma once
#include <string>
#include <vector>
#include "game/World.h"

enum class TaskType { RESTART_BUILD_SERVER, FIX_PRINTER_JAM, CLOSE_JIRA_TICKETS };

struct Task {
    TaskType type;
    std::string description;
    ObjectType targetObject;
    float interactDuration{3.0f};
    bool completed{false};
    bool inProgress{false};
    float progress{0};

    static Task create(TaskType t);
};

class TaskSystem {
public:
    std::vector<Task> tasks;

    void setTasks(const std::vector<TaskType>& types);
    void update(float dt, bool holdingKey, ObjectType nearObject);
    bool allComplete() const;
    Task* getActiveTask(ObjectType nearObject);
    void reset();
};
