#include "input.h"
#include "config.h"

namespace input {

static PressCallback s_callback  = nullptr;
static uint32_t      s_lastPress = 0;

struct ButtonDef { Button id; uint8_t pin; };
static const ButtonDef BUTTONS[] = {
    { Button::A, HW::BTN_A },
    { Button::B, HW::BTN_B },
    { Button::C, HW::BTN_C },
};

void init() {
    for (const auto& b : BUTTONS) {
        pinMode(b.pin, INPUT_PULLUP);
    }
}

void poll() {
    uint32_t now = millis();
    if (now - s_lastPress < Input::DEBOUNCE_MS) return;

    for (const auto& b : BUTTONS) {
        if (digitalRead(b.pin) == LOW) {
            s_lastPress = now;
            if (s_callback) s_callback(b.id);
            return; // one event per poll cycle
        }
    }
}

void onPress(PressCallback cb) { s_callback = cb; }

} // namespace input
