#include "config.h"
#include "pet.h"
#include "input.h"
#include "audio.h"
#include "renderer.h"

// ─── Input → action wiring ───────────────────────────────────────────────────
static void onButton(input::Button btn) {
    switch (btn) {
        case input::Button::A:
            pet::feed();
            audio::play(audio::Sound::FEED);
            break;
        case input::Button::B:
            pet::play();
            audio::play(audio::Sound::PLAY);
            break;
        case input::Button::C:
            pet::rest();
            audio::play(audio::Sound::REST);
            break;
    }
}

// ─── Critical-alert rate limiter ─────────────────────────────────────────────
static uint32_t s_lastAlertMs = 0;

static void checkCriticalAlert(const pet::Snapshot& snap) {
    bool critical = snap.vitals.hunger < Tuning::CRITICAL_THRESHOLD
                 || snap.vitals.joy    < Tuning::CRITICAL_THRESHOLD
                 || snap.vitals.energy < Tuning::CRITICAL_THRESHOLD;

    uint32_t now = millis();
    if (critical && now - s_lastAlertMs > Tuning::CRITICAL_ALERT_MS) {
        audio::play(audio::Sound::ALERT);
        s_lastAlertMs = now;
    }
}

// ─── Arduino entry points ─────────────────────────────────────────────────────
void setup() {
    input::init();
    audio::init();
    renderer::init();
    pet::init();

    input::onPress(onButton);
    pet::onChanged(renderer::markDirty);

    renderer::splash("Boing! Booting...", 800);
    audio::play(audio::Sound::BOOT);

    renderer::markDirty();
}

void loop() {
    input::poll();
    pet::tick(millis());
    audio::tick();

    pet::Snapshot snap = pet::snapshot();
    checkCriticalAlert(snap);
    renderer::frame(snap);

    delay(FRAME_MS);
}
