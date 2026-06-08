#include "TaskSystem.h"

Task Task::create(TaskType t) {
    Task task;
    task.type = t;
    task.interactDuration = 3.0f;
    task.completed = false;
    task.inProgress = false;
    task.progress = 0.0f;
    switch (t) {
        case TaskType::RESTART_BUILD_SERVER:
            task.description  = "Zrestartuj serwer buildow";
            task.targetObject = ObjectType::SERVER_RACK;
            break;
        case TaskType::FIX_PRINTER_JAM:
            task.description  = "Napraw zaciecie drukarki";
            task.targetObject = ObjectType::PRINTER;
            break;
        case TaskType::CLOSE_JIRA_TICKETS:
            task.description  = "Zamknij 3 zgloszenia Jira";
            task.targetObject = ObjectType::TERMINAL;
            break;
        default:
            task.description  = "Nieznane zadanie";
            task.targetObject = ObjectType::NONE;
            break;
    }
    return task;
}

void TaskSystem::setTasks(const std::vector<TaskType>& types) {
    tasks.clear();
    for (auto t : types) {
        tasks.push_back(Task::create(t));
    }
}

void TaskSystem::update(float dt, bool holdingKey, ObjectType nearObject) {
    for (auto& task : tasks) {
        if (task.completed) continue;
        if (task.targetObject == nearObject) {
            task.inProgress = holdingKey;
            if (holdingKey) {
                task.progress += dt / task.interactDuration;
                if (task.progress >= 1.0f) {
                    task.progress  = 1.0f;
                    task.completed = true;
                    task.inProgress = false;
                }
            } else {
                task.progress = 0.0f;
            }
        } else {
            task.inProgress = false;
        }
    }
}

bool TaskSystem::allComplete() const {
    for (const auto& t : tasks) {
        if (!t.completed) return false;
    }
    return true;
}

Task* TaskSystem::getActiveTask(ObjectType nearObject) {
    for (auto& t : tasks) {
        if (!t.completed && t.targetObject == nearObject) return &t;
    }
    return nullptr;
}

void TaskSystem::reset() {
    tasks.clear();
}
