#pragma once
#include <string>

class Renderer;
struct SuspicionSystem;
class DayPhase;

class HUD {
public:
    void render(Renderer& renderer, const SuspicionSystem& suspicion,
                const DayPhase& phase, const std::string& interactPrompt,
                const std::string& objective, bool showCrosshair);
private:
    void renderSuspicionBars(Renderer& r, const SuspicionSystem& s);
    void renderPhaseTimer(Renderer& r, const DayPhase& p);
    void renderPrompt(Renderer& r, const std::string& prompt);
    void renderObjective(Renderer& r, const std::string& obj);
    void renderCrosshair(Renderer& r);
};
