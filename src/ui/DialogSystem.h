#pragma once
#include <string>
#include <map>
#include "DialogData.h"

class DialogSystem {
public:
    DialogSystem();

    bool isActive() const { return m_active; }
    void startDialog(const std::string& startNodeId);
    void endDialog();
    void selectChoice(int index);

    const DialogNode* currentNode() const;
    float getSuspicionDelta() const { return m_pendingSuspicion; }
    void clearSuspicionDelta() { m_pendingSuspicion = 0.0f; }

    void render(class Renderer& r);

private:
    bool m_active{false};
    std::string m_currentNodeId;
    std::map<std::string, DialogNode> m_data;
    float m_pendingSuspicion{0};
};
