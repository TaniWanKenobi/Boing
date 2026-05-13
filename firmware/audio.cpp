#include "audio.h"
#include "config.h"

namespace audio {

struct Note { uint16_t freq; uint16_t durationMs; };
struct Melody { const Note* notes; uint8_t count; };

// freq=0 means silence (rest note)
static const Note NOTES_FEED[]  = { {1047, 60}, {1319, 90} };
static const Note NOTES_PLAY[]  = { {1319, 50}, {1568, 50}, {1047, 90} };
static const Note NOTES_REST[]  = { {784,  90}, {659, 130} };
static const Note NOTES_ALERT[] = { {523, 200}, {0,   100}, {523, 200} };
static const Note NOTES_BOOT[]  = { {523, 80}, {659, 80}, {784, 80}, {1047, 140} };

static const Melody MELODIES[] = {
    { NOTES_FEED,  2 },
    { NOTES_PLAY,  3 },
    { NOTES_REST,  2 },
    { NOTES_ALERT, 3 },
    { NOTES_BOOT,  4 },
};

static const Melody* s_active  = nullptr;
static uint8_t       s_noteIdx = 0;
static uint32_t      s_noteEnd = 0;

void init() {
    pinMode(HW::BUZZER, OUTPUT);
}

void play(Sound s) {
    s_active  = &MELODIES[static_cast<uint8_t>(s)];
    s_noteIdx = 0;
    s_noteEnd = 0; // force immediate start on next tick()
}

void tick() {
    if (!s_active) return;

    uint32_t now = millis();
    if (now < s_noteEnd) return;

    if (s_noteIdx >= s_active->count) {
        noTone(HW::BUZZER);
        s_active = nullptr;
        return;
    }

    const Note& n = s_active->notes[s_noteIdx++];
    if (n.freq > 0) {
        tone(HW::BUZZER, n.freq, n.durationMs);
    } else {
        noTone(HW::BUZZER);
    }
    s_noteEnd = now + n.durationMs;
}

} // namespace audio
