#pragma once
#include <Arduino.h>

namespace HW {
    static const uint8_t BTN_A      = D0;   // SW1 — Feed
    static const uint8_t BTN_B      = D1;   // SW2 — Play
    static const uint8_t BTN_C      = D2;   // SW3 — Rest
    static const uint8_t BUZZER     = D3;   // active-low sink
    static const uint8_t OLED_ADDR  = 0x3C;
}

namespace Display {
    static constexpr uint8_t WIDTH  = 128;
    static constexpr uint8_t HEIGHT = 64;
}

// Game-balance constants — edit freely to retune feel
namespace Tuning {
    static constexpr uint32_t DECAY_INTERVAL_MS  = 5000;
    static constexpr float    DECAY_BASE         = 0.010f; // per tick at full saturation
    static constexpr float    DECAY_EXPONENT     = 1.5f;   // accelerates as stat drops
    static constexpr float    FEED_DELTA         = 0.10f;
    static constexpr float    PLAY_JOY_DELTA     = 0.10f;
    static constexpr float    PLAY_ENERGY_COST   = 0.05f;
    static constexpr float    REST_DELTA         = 0.15f;
    static constexpr float    CRITICAL_THRESHOLD = 0.20f;  // triggers blink + alert
    static constexpr float    WELLNESS_HUNGER_W  = 0.40f;
    static constexpr float    WELLNESS_JOY_W     = 0.35f;
    static constexpr float    WELLNESS_ENERGY_W  = 0.25f;
    static constexpr uint32_t JUVENILE_AGE_S     = 300;    //  5 min — egg hatches
    static constexpr uint32_t ADULT_AGE_S        = 1200;   // 20 min — adolescent → adult
    static constexpr uint32_t ELDER_AGE_S        = 3600;   // 60 min — adult → elder
    static constexpr uint32_t CRITICAL_ALERT_MS  = 10000;  // alert repeat interval
}

namespace Input {
    static constexpr uint32_t DEBOUNCE_MS = 200;
}

static constexpr uint32_t FRAME_MS = 100;
