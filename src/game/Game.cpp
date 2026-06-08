#include "Game.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

Game::Game(SDL_Window* window, int w, int h)
    : m_window(window)
{
    m_renderer  = std::make_unique<Renderer>();
    m_renderer->init(w, h);

    m_player    = std::make_unique<Player>();
    m_world     = std::make_unique<World>();
    m_suspicion = std::make_unique<SuspicionSystem>();
    m_phase     = std::make_unique<DayPhase>();
    m_tasks     = std::make_unique<TaskSystem>();
    m_dialog    = std::make_unique<DialogSystem>();
    m_hud       = std::make_unique<HUD>();

    initNPCs();
    startDay();

    memset(m_keys, 0, sizeof(m_keys));
}

Game::~Game() = default;

void Game::initNPCs() {
    m_npcs.clear();

    NPC grazyna(NPCType::GRAZYNA_HR, "Grazyna", {3.0f,0.0f,15.0f}, {0.8f,0.4f,0.6f});
    grazyna.patrolPoints.push_back({{3,0,15}, 2.0f});
    grazyna.patrolPoints.push_back({{3,0,5},  2.0f});
    m_npcs.push_back(std::move(grazyna));

    NPC kamil(NPCType::KAMIL_DEV, "Kamil", {10.0f,0.0f,3.0f}, {0.4f,0.6f,0.8f});
    kamil.patrolPoints.push_back({{10,0,3}, 5.0f});
    kamil.patrolPoints.push_back({{1,0,10}, 3.0f});
    m_npcs.push_back(std::move(kamil));

    NPC tomasz(NPCType::TOMASZ_MANAGER, "Tomasz", {17.0f,0.0f,15.0f}, {0.4f,0.7f,0.4f});
    tomasz.patrolPoints.push_back({{17,0,15}, 4.0f});
    tomasz.patrolPoints.push_back({{10,0,6},  2.0f});
    m_npcs.push_back(std::move(tomasz));
}

void Game::startDay() {
    m_briefing = m_eventGen.generate(m_phase->dayNumber, m_prevDayEvent);
    m_tasks->setTasks(m_briefing.workTasks);
    m_sabotageCompletedToday = false;
    m_sabotageProgress.cancel();
    m_suspicion->reset();
    m_state = GameState::BRIEFING;
    m_stateTimer = 0;

    // Reset phase time but keep day number
    m_phase->gameTimeHours = 8.0f;
    m_phase->current = PhaseType::WORK;
}

void Game::handleEvent(const SDL_Event& e) {
    switch (e.type) {
        case SDL_QUIT:
            m_quit = true;
            break;

        case SDL_KEYDOWN: {
            SDL_Scancode sc = e.key.keysym.scancode;
            if (sc < SDL_NUM_SCANCODES) m_keys[sc] = true;

            if (m_state == GameState::MENU) {
                if (sc == SDL_SCANCODE_RETURN || sc == SDL_SCANCODE_RETURN2) {
                    startDay();
                }
                if (sc == SDL_SCANCODE_ESCAPE) m_quit = true;
            } else if (m_state == GameState::BRIEFING) {
                m_state = GameState::PLAYING;
                SDL_SetRelativeMouseMode(SDL_TRUE);
            } else if (m_state == GameState::PLAYING) {
                if (sc == SDL_SCANCODE_ESCAPE) {
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    m_state = GameState::MENU;
                }
                if (sc == SDL_SCANCODE_E) m_holdingE = true;
                if (sc == SDL_SCANCODE_F) {
                    m_holdingF = true;
                    if (m_nearNPC && !m_dialog->isActive()) {
                        m_dialog->startDialog(m_nearNPC->dialogId);
                        m_nearNPC->startTalking();
                        m_state = GameState::DIALOGUE;
                    }
                }
            } else if (m_state == GameState::DIALOGUE) {
                if (sc == SDL_SCANCODE_1) m_dialog->selectChoice(0);
                else if (sc == SDL_SCANCODE_2) m_dialog->selectChoice(1);
                else if (sc == SDL_SCANCODE_3) m_dialog->selectChoice(2);
                else if (sc == SDL_SCANCODE_ESCAPE) {
                    m_dialog->endDialog();
                }
                if (!m_dialog->isActive()) {
                    // Apply suspicion from dialog
                    float sd = m_dialog->getSuspicionDelta();
                    m_dialog->clearSuspicionDelta();
                    if (sd > 0) m_suspicion->addSuspicion(SuspicionGroup::TEAM, sd);
                    else if (sd < 0) m_suspicion->addSuspicion(SuspicionGroup::TEAM, sd);
                    // Stop talking NPC
                    for (auto& npc : m_npcs) {
                        if (npc.state == NPCState::TALKING) npc.stopTalking();
                    }
                    m_state = GameState::PLAYING;
                }
            } else if (m_state == GameState::CAUGHT || m_state == GameState::DAY_END) {
                if (m_state == GameState::DAY_END) {
                    m_phase->advanceToNextDay();
                    initNPCs();
                    startDay();
                } else {
                    // Caught: restart day
                    m_suspicion->reset();
                    m_phase->gameTimeHours = 8.0f;
                    m_phase->current = PhaseType::WORK;
                    initNPCs();
                    startDay();
                }
            }
            break;
        }

        case SDL_KEYUP: {
            SDL_Scancode sc = e.key.keysym.scancode;
            if (sc < SDL_NUM_SCANCODES) m_keys[sc] = false;
            if (sc == SDL_SCANCODE_E) {
                m_holdingE = false;
                m_sabotageProgress.cancel();
            }
            if (sc == SDL_SCANCODE_F) m_holdingF = false;
            break;
        }

        case SDL_MOUSEMOTION:
            if (m_state == GameState::PLAYING || m_state == GameState::DIALOGUE) {
                m_player->handleMouseMove((float)e.motion.xrel, (float)e.motion.yrel);
            }
            break;

        default:
            break;
    }
}

void Game::checkInteractions() {
    glm::vec3 pos = m_player->getPosition();
    glm::vec3 dir = m_player->getForward();

    m_nearObject = m_world->getInteractable(pos, dir, m_player->interactRange);

    m_nearNPC = nullptr;
    float bestDist = m_player->dialogRange;
    for (auto& npc : m_npcs) {
        float d = npc.distanceTo(pos);
        if (d < bestDist) {
            bestDist = d;
            m_nearNPC = &npc;
        }
    }
}

void Game::updatePlaying(float dt) {
    // Move inputs
    float fwd = 0, right = 0;
    if (m_keys[SDL_SCANCODE_W]) fwd   += 1.0f;
    if (m_keys[SDL_SCANCODE_S]) fwd   -= 1.0f;
    if (m_keys[SDL_SCANCODE_A]) right -= 1.0f;
    if (m_keys[SDL_SCANCODE_D]) right += 1.0f;
    m_player->setMoveInput(fwd, right, false);
    m_player->update(dt, *m_world);

    glm::vec3 playerPos = m_player->getPosition();
    bool playerSabotaging = m_sabotageProgress.active;

    // Update NPCs
    for (auto& npc : m_npcs) {
        npc.update(dt, playerPos, playerSabotaging);
        if (npc.canSeePlayer(playerPos, playerSabotaging) && playerSabotaging) {
            m_suspicion->addSuspicion(SuspicionGroup::TEAM, 10.0f * dt);
            m_suspicion->addSuspicion(SuspicionGroup::SECURITY, 5.0f * dt);
        }
    }

    bool nearNPC = m_nearNPC != nullptr;
    m_suspicion->update(dt, nearNPC);
    m_phase->update(dt);

    checkInteractions();

    // Sabotage or task interaction
    ObjectType nearType = m_nearObject ? m_nearObject->type : ObjectType::NONE;

    if (m_phase->isSabotagePhase() && m_nearObject && !m_sabotageCompletedToday) {
        // Determine sabotage type from object
        SabotageType sabType = SabotageType::PRINTER_JAM;
        bool validSabTarget = false;
        switch (nearType) {
            case ObjectType::PRINTER:     sabType = SabotageType::PRINTER_JAM; validSabTarget = true; break;
            case ObjectType::TERMINAL:    sabType = SabotageType::COMMIT_BUG;  validSabTarget = true; break;
            case ObjectType::SERVER_RACK: sabType = SabotageType::AD_LOCKOUT;  validSabTarget = true; break;
            default: break;
        }
        if (validSabTarget && m_holdingE) {
            if (!m_sabotageProgress.active) {
                SabotageAction action = SabotageAction::create(sabType);
                m_sabotageProgress.start(sabType, action.holdDuration);
            }
            bool done = m_sabotageProgress.update(dt, true);
            if (done) {
                SabotageAction action = SabotageAction::create(sabType);
                m_suspicion->addSuspicion(SuspicionGroup::TEAM,     action.teamSuspicion);
                m_suspicion->addSuspicion(SuspicionGroup::BOSS,     action.bossSuspicion);
                m_suspicion->addSuspicion(SuspicionGroup::SECURITY, action.securitySuspicion);
                m_sabotageCompletedToday = true;
                m_prevDayEvent = action.nextDayEvent;
            }
        } else if (!m_holdingE) {
            m_sabotageProgress.cancel();
        }
    } else if (m_phase->isWorkPhase()) {
        // Work task
        if (m_holdingE && nearType != ObjectType::NONE) {
            m_tasks->update(dt, true, nearType);
        } else {
            m_tasks->update(dt, false, nearType);
        }
    }

    // Check caught
    if (m_suspicion->isCaught()) {
        SuspicionGroup cg = m_suspicion->getCaughtGroup();
        m_caughtByNPC = SuspicionSystem::groupName(cg);
        m_state = GameState::CAUGHT;
        SDL_SetRelativeMouseMode(SDL_FALSE);
        return;
    }

    // Check end of day
    if (m_phase->isEndOfDay()) {
        m_state = GameState::DAY_END;
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

void Game::update(float dt) {
    m_stateTimer += dt;
    switch (m_state) {
        case GameState::PLAYING:
            updatePlaying(dt);
            break;
        case GameState::DIALOGUE:
            // Still update player movement in dialog? No — freeze.
            // But do apply dialog suspicion deltas if pending
            if (!m_dialog->isActive()) {
                m_state = GameState::PLAYING;
            }
            break;
        default:
            break;
    }
}

void Game::render() {
    m_renderer->beginFrame();

    switch (m_state) {
        case GameState::MENU:
            renderMenu();
            break;
        case GameState::BRIEFING:
            renderBriefing();
            break;
        case GameState::PLAYING: {
            m_renderer->setCamera(m_player->getCamera());
            m_world->render(*m_renderer);
            for (auto& npc : m_npcs) npc.render(*m_renderer);
            m_renderer->flushGeometry();

            std::string prompt = getInteractPrompt();
            std::string obj    = getCurrentObjective();
            m_hud->render(*m_renderer, *m_suspicion, *m_phase, prompt, obj, true);

            // Show sabotage progress bar if active
            if (m_sabotageProgress.active) {
                float pct = m_sabotageProgress.progress;
                float bw  = 300.0f;
                float bx  = (m_renderer->screenW - bw) * 0.5f;
                float by  = m_renderer->screenH * 0.5f + 40.0f;
                m_renderer->drawRect2D(bx, by, bw, 16.0f, 0.1f, 0.1f, 0.1f, 0.8f);
                m_renderer->drawRect2D(bx, by, bw * pct, 16.0f, 1.0f, 0.6f, 0.0f, 1.0f);
                m_renderer->drawText2D(bx, by - 18.0f, 1.5f, 1.0f, 1.0f, 0.0f, "SABOTAZ...");
            }
            m_renderer->flushHUD();
            break;
        }
        case GameState::DIALOGUE: {
            m_renderer->setCamera(m_player->getCamera());
            m_world->render(*m_renderer);
            for (auto& npc : m_npcs) npc.render(*m_renderer);
            m_renderer->flushGeometry();

            std::string prompt = "";
            std::string obj    = getCurrentObjective();
            m_hud->render(*m_renderer, *m_suspicion, *m_phase, prompt, obj, false);
            m_renderer->flushHUD();

            m_dialog->render(*m_renderer);
            break;
        }
        case GameState::CAUGHT:
            renderCaught();
            break;
        case GameState::DAY_END:
            renderDayEnd();
            break;
        default:
            break;
    }

    m_renderer->endFrame();
    SDL_GL_SwapWindow(m_window);
}

void Game::renderMenu() {
    float sw = (float)m_renderer->screenW;
    float sh = (float)m_renderer->screenH;

    m_renderer->drawRect2D(0, 0, sw, sh, 0.05f, 0.05f, 0.1f, 1.0f);

    // Title
    float titleScale = 4.0f;
    float titleW = (float)strlen("9 TO 5 TO CHAOS") * 9.0f * titleScale;
    float titleX = (sw - titleW) * 0.5f;
    m_renderer->drawText2D(titleX, sh * 0.35f, titleScale, 1.0f, 0.8f, 0.0f, "9 TO 5 TO CHAOS");

    // Subtitle
    float subScale = 2.0f;
    const char* sub = "Nacisnij ENTER aby zaczac";
    float subW = (float)strlen(sub) * 9.0f * subScale;
    m_renderer->drawText2D((sw - subW) * 0.5f, sh * 0.55f, subScale, 0.8f, 0.8f, 0.8f, sub);

    const char* sub2 = "WASD - ruch, mysz - oglad, E - akcja, F - rozmowa";
    float sub2W = (float)strlen(sub2) * 9.0f * 1.5f;
    m_renderer->drawText2D((sw - sub2W) * 0.5f, sh * 0.65f, 1.5f, 0.6f, 0.6f, 0.6f, sub2);

    m_renderer->flushHUD();
}

void Game::renderBriefing() {
    float sw = (float)m_renderer->screenW;
    float sh = (float)m_renderer->screenH;

    m_renderer->drawRect2D(0, 0, sw, sh, 0.05f, 0.05f, 0.08f, 1.0f);

    char buf[128];
    snprintf(buf, sizeof(buf), "DZIEN %d - ODPRAWY PORANNE", m_briefing.dayNumber);
    m_renderer->drawText2D(40.0f, 40.0f, 2.5f, 1.0f, 0.8f, 0.0f, buf);

    m_renderer->drawText2D(40.0f, 100.0f, 2.0f, 0.9f, 0.9f, 0.9f, "ZADANIA DNIA:");

    float y = 135.0f;
    for (auto& t : m_briefing.workTasks) {
        Task tmp = Task::create(t);
        std::string line = "  - " + tmp.description;
        m_renderer->drawText2D(40.0f, y, 1.5f, 0.8f, 1.0f, 0.8f, line.c_str());
        y += 22.0f;
    }

    y += 15.0f;
    m_renderer->drawText2D(40.0f, y, 1.5f, 0.9f, 0.9f, 0.3f, "SUGESTIA SABOTAZU:");
    y += 20.0f;
    m_renderer->drawText2D(40.0f, y, 1.5f, 0.7f, 0.9f, 0.7f, m_briefing.sabotageSuggestion.c_str());

    if (!m_briefing.specialEvent.empty()) {
        y += 30.0f;
        m_renderer->drawRect2D(35.0f, y - 5.0f, sw - 70.0f, 30.0f, 0.4f, 0.0f, 0.0f, 0.7f);
        m_renderer->drawText2D(40.0f, y, 1.5f, 1.0f, 0.3f, 0.3f, m_briefing.specialEvent.c_str());
    }

    m_renderer->drawText2D(40.0f, sh - 50.0f, 1.5f, 0.5f, 0.5f, 0.5f, "Nacisnij dowolny klawisz aby zaczac...");

    m_renderer->flushHUD();
}

void Game::renderCaught() {
    float sw = (float)m_renderer->screenW;
    float sh = (float)m_renderer->screenH;

    m_renderer->drawRect2D(0, 0, sw, sh, 0.3f, 0.0f, 0.0f, 0.9f);

    const char* title = "ZOSTALES PRZYLAPANY!";
    float tw = (float)strlen(title) * 9.0f * 3.0f;
    m_renderer->drawText2D((sw - tw) * 0.5f, sh * 0.3f, 3.0f, 1.0f, 0.2f, 0.2f, title);

    char buf[128];
    snprintf(buf, sizeof(buf), "Przylapal Cie: %s", m_caughtByNPC.c_str());
    float bw = (float)strlen(buf) * 9.0f * 2.0f;
    m_renderer->drawText2D((sw - bw) * 0.5f, sh * 0.45f, 2.0f, 1.0f, 0.8f, 0.8f, buf);

    const char* sub = "Nacisnij dowolny klawisz aby sprobowac ponownie";
    float sw2 = (float)strlen(sub) * 9.0f * 1.5f;
    m_renderer->drawText2D((sw - sw2) * 0.5f, sh * 0.65f, 1.5f, 0.7f, 0.7f, 0.7f, sub);

    m_renderer->flushHUD();
}

void Game::renderDayEnd() {
    float sw = (float)m_renderer->screenW;
    float sh = (float)m_renderer->screenH;

    m_renderer->drawRect2D(0, 0, sw, sh, 0.05f, 0.1f, 0.05f, 0.95f);

    char buf[128];
    snprintf(buf, sizeof(buf), "KONIEC DNIA %d", m_phase->dayNumber);
    float tw = (float)strlen(buf) * 9.0f * 3.0f;
    m_renderer->drawText2D((sw - tw) * 0.5f, 60.0f, 3.0f, 0.8f, 1.0f, 0.4f, buf);

    m_renderer->drawText2D(60.0f, 150.0f, 2.0f, 0.9f, 0.9f, 0.9f, "PODSUMOWANIE:");

    char sab[64];
    snprintf(sab, sizeof(sab), "Sabotaz: %s", m_sabotageCompletedToday ? "WYKONANY" : "NIEWYKONANY");
    m_renderer->drawText2D(60.0f, 185.0f, 1.5f,
        m_sabotageCompletedToday ? 0.3f : 0.9f,
        m_sabotageCompletedToday ? 0.9f : 0.3f,
        0.3f, sab);

    m_renderer->drawText2D(60.0f, 215.0f, 1.5f, 0.9f, 0.9f, 0.9f, "POZIOMY PODEJRZEN:");

    const char* groups[] = {"BOSS", "TEAM", "SEC "};
    SuspicionGroup gids[] = {SuspicionGroup::BOSS, SuspicionGroup::TEAM, SuspicionGroup::SECURITY};
    for (int i = 0; i < 3; ++i) {
        float val = m_suspicion->get(gids[i]);
        snprintf(buf, sizeof(buf), "  %s: %.0f%%", groups[i], val);
        m_renderer->drawText2D(60.0f, 235.0f + i * 20.0f, 1.5f, 0.8f, 0.8f, 1.0f, buf);
    }

    const char* sub = "Nacisnij dowolny klawisz aby kontynuowac";
    float sw2 = (float)strlen(sub) * 9.0f * 1.5f;
    m_renderer->drawText2D((sw - sw2) * 0.5f, sh - 50.0f, 1.5f, 0.5f, 0.5f, 0.5f, sub);

    m_renderer->flushHUD();
}

std::string Game::getInteractPrompt() const {
    if (!m_nearObject && !m_nearNPC) return "";

    if (m_nearNPC) {
        return "[F] Porozmawiaj z " + m_nearNPC->name;
    }

    if (m_nearObject) {
        if (m_phase->isWorkPhase()) {
            Task* t = m_tasks->getActiveTask(m_nearObject->type);
            if (t && !t->completed) {
                if (m_holdingE && t->inProgress) {
                    int pct = static_cast<int>(t->progress * 100.0f);
                    return "[E] " + t->description + " - " + std::to_string(pct) + "%";
                }
                float dur = t->interactDuration;
                return "[E przytrzymaj " + std::to_string(static_cast<int>(dur)) +
                       " sek] " + t->description;
            }
            // Object present but no matching task right now
            return m_nearObject->label + " (nie potrzebne teraz)";
        } else if (m_phase->isSabotagePhase() && !m_sabotageCompletedToday) {
            const char* action = nullptr;
            float dur = 0;
            switch (m_nearObject->type) {
                case ObjectType::PRINTER:
                    action = "Zaciej drukarke"; dur = 2.0f; break;
                case ObjectType::TERMINAL:
                    action = "Zatwierdz blad w kodzie"; dur = 3.0f; break;
                case ObjectType::SERVER_RACK:
                    action = "Zablokuj Active Directory"; dur = 4.0f; break;
                default: break;
            }
            if (action) {
                if (m_holdingE && m_sabotageProgress.active) {
                    int pct = static_cast<int>(m_sabotageProgress.progress * 100.0f);
                    return std::string("[E] ") + action + " - " + std::to_string(pct) + "%";
                }
                return "[E przytrzymaj " + std::to_string(static_cast<int>(dur)) +
                       " sek] " + action;
            }
        }
        return m_nearObject->label;
    }
    return "";
}

std::string Game::getCurrentObjective() const {
    if (m_phase->isWorkPhase()) {
        for (const auto& t : m_tasks->tasks) {
            if (!t.completed) {
                // Add location hint per task type
                const char* where = "";
                switch (t.targetObject) {
                    case ObjectType::SERVER_RACK:   where = " -> Pokoj serwerowy"; break;
                    case ObjectType::PRINTER:        where = " -> Drukarka (prawy rog)"; break;
                    case ObjectType::TERMINAL:       where = " -> Biurko Kamila"; break;
                    default: break;
                }
                return "PRACA: " + t.description + where;
            }
        }
        return "Zadania ukonczone! Czekaj na godz. 12:00.";
    } else if (m_phase->isSabotagePhase()) {
        if (m_sabotageCompletedToday) return "Sabotaz wykonany! Koniec o 17:00.";
        return "SABOTAZ: Drukarka / Terminal / Serwer - przytrzymaj E";
    }
    return "Koniec dnia";
}
