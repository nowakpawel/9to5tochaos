#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <vector>
#include "game/Player.h"
#include "game/World.h"
#include "game/NPC.h"
#include "game/SuspicionSystem.h"
#include "game/SabotageAction.h"
#include "game/TaskSystem.h"
#include "game/DayPhase.h"
#include "game/RandomEventGenerator.h"
#include "render/Renderer.h"
#include "render/HUD.h"
#include "ui/DialogSystem.h"

enum class GameState {
    MENU, BRIEFING, PLAYING, DIALOGUE, CAUGHT, DAY_END
};

class Game {
public:
    Game(SDL_Window* window, int w, int h);
    ~Game();

    void update(float dt);
    void render();
    void handleEvent(const SDL_Event& e);
    bool shouldQuit() const { return m_quit; }

private:
    SDL_Window* m_window;
    bool m_quit{false};
    GameState m_state{GameState::MENU};

    std::unique_ptr<Renderer>       m_renderer;
    std::unique_ptr<Player>         m_player;
    std::unique_ptr<World>          m_world;
    std::unique_ptr<SuspicionSystem> m_suspicion;
    std::unique_ptr<DayPhase>       m_phase;
    std::unique_ptr<TaskSystem>     m_tasks;
    std::unique_ptr<DialogSystem>   m_dialog;
    std::unique_ptr<HUD>            m_hud;
    RandomEventGenerator            m_eventGen;

    std::vector<NPC>    m_npcs;
    DailyBriefing       m_briefing;

    const WorldObject*  m_nearObject{nullptr};
    NPC*                m_nearNPC{nullptr};
    SabotageProgress    m_sabotageProgress;
    bool m_holdingE{false};
    bool m_holdingF{false};

    std::string m_caughtByNPC;
    bool        m_sabotageCompletedToday{false};
    std::string m_prevDayEvent;

    float m_stateTimer{0};

    void initNPCs();
    void startDay();
    void checkInteractions();
    void updatePlaying(float dt);
    void renderMenu();
    void renderBriefing();
    void renderCaught();
    void renderDayEnd();

    std::string getInteractPrompt() const;
    std::string getCurrentObjective() const;

    bool m_keys[SDL_NUM_SCANCODES]{};
};
