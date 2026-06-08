#include "SabotageAction.h"

SabotageAction SabotageAction::create(SabotageType t) {
    SabotageAction a;
    a.type = t;
    switch (t) {
        case SabotageType::PRINTER_JAM:
            a.name            = "Zaciecie drukarki";
            a.description     = "Wkladasz papier bokiem do drukarki";
            a.holdDuration    = 2.0f;
            a.teamSuspicion   = 5.0f;
            a.bossSuspicion   = 2.0f;
            a.securitySuspicion = 0.0f;
            a.nextDayEvent    = "printer_jam";
            break;
        case SabotageType::COMMIT_BUG:
            a.name            = "Zatwierdzenie bledu";
            a.description     = "Komentujesz kluczowa linie w kodzie produkcyjnym";
            a.holdDuration    = 3.0f;
            a.teamSuspicion   = 15.0f;
            a.bossSuspicion   = 5.0f;
            a.securitySuspicion = 0.0f;
            a.nextDayEvent    = "critical_bug";
            break;
        case SabotageType::AD_LOCKOUT:
            a.name            = "Blokada Active Directory";
            a.description     = "Resetujesz konta w AD i gubisz backup";
            a.holdDuration    = 4.0f;
            a.teamSuspicion   = 0.0f;
            a.bossSuspicion   = 10.0f;
            a.securitySuspicion = 20.0f;
            a.nextDayEvent    = "ad_lockout";
            break;
        default:
            a.name            = "Nieznana akcja";
            a.description     = "";
            a.holdDuration    = 2.0f;
            a.teamSuspicion   = 0.0f;
            a.bossSuspicion   = 0.0f;
            a.securitySuspicion = 0.0f;
            a.nextDayEvent    = "";
            break;
    }
    return a;
}

void SabotageProgress::start(SabotageType t, float duration) {
    type         = t;
    holdDuration = duration;
    progress     = 0.0f;
    active       = true;
}

bool SabotageProgress::update(float dt, bool holdingKey) {
    if (!active) return false;
    if (holdingKey) {
        if (holdDuration > 0.0f) progress += dt / holdDuration;
        if (progress >= 1.0f) {
            progress = 1.0f;
            active   = false;
            return true;
        }
    } else {
        progress = 0.0f;
    }
    return false;
}

void SabotageProgress::cancel() {
    active   = false;
    progress = 0.0f;
}
