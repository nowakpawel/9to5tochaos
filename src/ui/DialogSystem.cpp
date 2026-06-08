#include "DialogSystem.h"
#include "render/Renderer.h"
#include <cstdio>

DialogSystem::DialogSystem() {
    m_data = buildDialogData();
}

void DialogSystem::startDialog(const std::string& startNodeId) {
    if (m_data.count(startNodeId)) {
        m_currentNodeId = startNodeId;
        m_active = true;
        m_pendingSuspicion = 0;
    }
}

void DialogSystem::endDialog() {
    m_active = false;
    m_currentNodeId = "";
}

void DialogSystem::selectChoice(int index) {
    const DialogNode* node = currentNode();
    if (!node) return;
    if (index < 0 || index >= (int)node->choices.size()) return;

    const DialogChoice& choice = node->choices[index];
    m_pendingSuspicion += choice.suspicionDelta;

    if (choice.nextNode.empty()) {
        endDialog();
    } else {
        m_currentNodeId = choice.nextNode;
    }
}

const DialogNode* DialogSystem::currentNode() const {
    if (!m_active) return nullptr;
    auto it = m_data.find(m_currentNodeId);
    if (it == m_data.end()) return nullptr;
    return &it->second;
}

void DialogSystem::render(Renderer& r) {
    if (!m_active) return;
    const DialogNode* node = currentNode();
    if (!node) return;

    float sw = (float)r.screenW;
    float sh = (float)r.screenH;

    // Background box (bottom 200px)
    float boxY = sh - 200.0f;
    r.drawRect2D(0, boxY, sw, 200.0f, 0.0f, 0.0f, 0.0f, 0.75f);
    r.drawRect2D(0, boxY, sw, 2.0f, 0.8f, 0.8f, 0.2f, 1.0f);

    // Portrait (colored rect)
    r.drawRect2D(10.0f, boxY + 10.0f, 80.0f, 80.0f, 0.4f, 0.4f, 0.6f, 1.0f);

    // NPC name
    r.drawText2D(100.0f, boxY + 12.0f, 2.0f, 1.0f, 1.0f, 0.2f, node->speaker.c_str());

    // Dialog text (wrap roughly at 80 chars)
    std::string txt = node->text;
    float lineY = boxY + 35.0f;
    size_t start = 0;
    while (start < txt.size()) {
        size_t end = start + 70;
        if (end >= txt.size()) end = txt.size();
        else {
            size_t space = txt.rfind(' ', end);
            if (space != std::string::npos && space > start) end = space;
        }
        std::string line = txt.substr(start, end - start);
        r.drawText2D(100.0f, lineY, 1.5f, 0.95f, 0.95f, 0.95f, line.c_str());
        lineY += 14.0f;
        start = end;
        if (start < txt.size() && txt[start] == ' ') start++;
    }

    // Choices
    float choiceY = boxY + 110.0f;
    for (int i = 0; i < (int)node->choices.size(); ++i) {
        char prefix[8];
        snprintf(prefix, sizeof(prefix), "[%d] ", i+1);
        std::string choiceText = prefix + node->choices[i].text;
        r.drawText2D(20.0f, choiceY, 1.5f, 0.8f, 1.0f, 0.8f, choiceText.c_str());
        choiceY += 20.0f;
    }

    r.flushHUD();
}
