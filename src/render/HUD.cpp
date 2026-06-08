#include "HUD.h"
#include "Renderer.h"
#include "game/SuspicionSystem.h"
#include "game/DayPhase.h"
#include <cstring>
#include <cstdio>
#include <string>

void HUD::render(Renderer& r, const SuspicionSystem& suspicion,
                 const DayPhase& phase, const std::string& interactPrompt,
                 const std::string& objective, bool showCrosshair) {
    renderSuspicionBars(r, suspicion);
    renderPhaseTimer(r, phase);
    if (!interactPrompt.empty()) renderPrompt(r, interactPrompt);
    if (!objective.empty()) renderObjective(r, objective);
    if (showCrosshair) renderCrosshair(r);
}

void HUD::renderSuspicionBars(Renderer& r, const SuspicionSystem& s) {
    // Top-right corner, 3 bars
    const float barW = 200.0f;
    const float barH = 18.0f;
    const float startX = r.screenW - barW - 10.0f;
    const float startY = 10.0f;
    const float spacing = 24.0f;

    struct BarDef { float fr,fg,fb; SuspicionGroup g; const char* label; };
    BarDef bars[3] = {
        {0.9f,0.2f,0.2f, SuspicionGroup::BOSS,     "BOSS"},
        {0.9f,0.9f,0.2f, SuspicionGroup::TEAM,     "TEAM"},
        {0.2f,0.4f,0.9f, SuspicionGroup::SECURITY, "SEC "},
    };

    for (int i = 0; i < 3; ++i) {
        float y = startY + i * spacing;
        // background
        r.drawRect2D(startX, y, barW, barH, 0.15f, 0.15f, 0.15f, 0.8f);
        // filled portion
        float pct = s.get(bars[i].g) / 100.0f;
        if (pct > 1.0f) pct = 1.0f;
        r.drawRect2D(startX, y, barW * pct, barH, bars[i].fr, bars[i].fg, bars[i].fb, 0.9f);
        // label
        r.drawText2D(startX - 40.0f, y + 4.0f, 1.5f, 1.0f, 1.0f, 1.0f, bars[i].label);
    }
}

void HUD::renderPhaseTimer(Renderer& r, const DayPhase& p) {
    // Top-left
    char buf[64];
    std::string phaseName = p.getPhaseNamePolish();
    int hh = p.getHour();
    int mm = p.getMinute();
    snprintf(buf, sizeof(buf), "%s %02d:%02d", phaseName.c_str(), hh, mm);
    r.drawRect2D(5.0f, 5.0f, 200.0f, 22.0f, 0.0f, 0.0f, 0.0f, 0.6f);
    r.drawText2D(10.0f, 9.0f, 1.5f, 1.0f, 1.0f, 0.8f, buf);
}

void HUD::renderPrompt(Renderer& r, const std::string& prompt) {
    float screenW = (float)r.screenW;
    float screenH = (float)r.screenH;
    float boxW = 400.0f;
    float boxH = 30.0f;
    float x = (screenW - boxW) * 0.5f;
    float y = screenH - 80.0f;
    r.drawRect2D(x, y, boxW, boxH, 0.0f, 0.0f, 0.0f, 0.7f);
    r.drawText2D(x + 10.0f, y + 7.0f, 1.5f, 1.0f, 1.0f, 0.0f, prompt.c_str());
}

void HUD::renderObjective(Renderer& r, const std::string& obj) {
    float y = (float)r.screenH - 40.0f;
    r.drawRect2D(5.0f, y, 350.0f, 22.0f, 0.0f, 0.0f, 0.0f, 0.6f);
    r.drawText2D(10.0f, y + 4.0f, 1.5f, 0.8f, 1.0f, 0.8f, obj.c_str());
}

void HUD::renderCrosshair(Renderer& r) {
    float cx = r.screenW * 0.5f;
    float cy = r.screenH * 0.5f;
    // horizontal bar
    r.drawRect2D(cx - 10.0f, cy - 1.0f, 20.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    // vertical bar
    r.drawRect2D(cx - 1.0f, cy - 10.0f, 2.0f, 20.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}
