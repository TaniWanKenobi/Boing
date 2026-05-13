#include "renderer.h"
#include "config.h"
#include "sprites.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace renderer {

// All pixel coordinates in one place — no magic numbers in draw calls.
namespace Layout {
    static constexpr uint8_t SPRITE_X    = 56;
    static constexpr uint8_t SPRITE_Y    = 2;
    static constexpr uint8_t LABEL_X     = 0;
    static constexpr uint8_t BAR_X       = 24;
    static constexpr uint8_t BAR_W       = 100;
    static constexpr uint8_t BAR_H       = 6;
    static constexpr uint8_t ROW_HUNGER  = 24;
    static constexpr uint8_t ROW_JOY     = 34;
    static constexpr uint8_t ROW_ENERGY  = 44;
    static constexpr uint8_t ROW_HINTS   = 56;
    static constexpr uint8_t HATCH_BAR_X = 14;
    static constexpr uint8_t HATCH_BAR_Y = 40;
    static constexpr uint8_t HATCH_BAR_W = 100;
    static constexpr uint8_t HATCH_BAR_H = 8;
}

static Adafruit_SSD1306 s_display(Display::WIDTH, Display::HEIGHT, &Wire, -1);
static bool             s_dirty          = true;
static bool             s_lastBlinkState = false;

void init() {
    s_display.begin(SSD1306_SWITCHCAPVCC, HW::OLED_ADDR);
    s_display.setTextColor(SSD1306_WHITE);
    s_display.setTextSize(1);
    s_display.clearDisplay();
    s_display.display();
}

void splash(const char* msg, uint16_t durationMs) {
    s_display.clearDisplay();
    s_display.setCursor(0, 0);
    s_display.print(msg);
    s_display.display();
    delay(durationMs);
}

static void drawStatBar(uint8_t x, uint8_t y, float value) {
    uint8_t fill = static_cast<uint8_t>(value * Layout::BAR_W);
    s_display.drawRect(x, y, Layout::BAR_W, Layout::BAR_H, SSD1306_WHITE);
    if (fill > 0) {
        s_display.fillRect(x, y, fill, Layout::BAR_H, SSD1306_WHITE);
    }
}

static void renderHatchling(const pet::Snapshot& snap) {
    s_display.setCursor(36, 2);
    s_display.print("EGG");

    s_display.drawBitmap(Layout::SPRITE_X, Layout::SPRITE_Y,
                         spr_egg, SPR_W, SPR_H, SSD1306_WHITE);

    s_display.setCursor(0, 32);
    s_display.print("Hatching:");

    float progress = static_cast<float>(snap.ageSecs)
                   / static_cast<float>(Tuning::JUVENILE_AGE_S);
    if (progress > 1.0f) progress = 1.0f;

    uint8_t fill = static_cast<uint8_t>(progress * Layout::HATCH_BAR_W);
    s_display.drawRect(Layout::HATCH_BAR_X, Layout::HATCH_BAR_Y,
                       Layout::HATCH_BAR_W, Layout::HATCH_BAR_H, SSD1306_WHITE);
    if (fill > 0) {
        s_display.fillRect(Layout::HATCH_BAR_X, Layout::HATCH_BAR_Y,
                           fill, Layout::HATCH_BAR_H, SSD1306_WHITE);
    }
}

static void renderPet(const pet::Snapshot& snap, bool blinkHide) {
    uint8_t stageIdx = static_cast<uint8_t>(snap.stage);
    uint8_t moodIdx  = static_cast<uint8_t>(snap.mood);

    if (!blinkHide) {
        s_display.drawBitmap(Layout::SPRITE_X, Layout::SPRITE_Y,
                             SPRITE_TABLE[stageIdx][moodIdx],
                             SPR_W, SPR_H, SSD1306_WHITE);
    }

    s_display.setCursor(Layout::LABEL_X, Layout::ROW_HUNGER);
    s_display.print("HUN");
    drawStatBar(Layout::BAR_X, Layout::ROW_HUNGER, snap.vitals.hunger);

    s_display.setCursor(Layout::LABEL_X, Layout::ROW_JOY);
    s_display.print("JOY");
    drawStatBar(Layout::BAR_X, Layout::ROW_JOY, snap.vitals.joy);

    s_display.setCursor(Layout::LABEL_X, Layout::ROW_ENERGY);
    s_display.print("ENG");
    drawStatBar(Layout::BAR_X, Layout::ROW_ENERGY, snap.vitals.energy);

    // Elder overlay
    if (snap.stage == pet::LifeStage::ELDER) {
        s_display.setCursor(Display::WIDTH - 24, 2);
        s_display.print("zzz");
    }

    s_display.setCursor(0, Layout::ROW_HINTS);
    s_display.print("[A]Feed [B]Play [C]Rest");
}

void markDirty() { s_dirty = true; }

void frame(const pet::Snapshot& snap) {
    // Keep the dirty flag set while critical blinking is active.
    bool critical = snap.vitals.hunger < Tuning::CRITICAL_THRESHOLD
                 || snap.vitals.joy    < Tuning::CRITICAL_THRESHOLD
                 || snap.vitals.energy < Tuning::CRITICAL_THRESHOLD;

    if (critical) {
        bool blinkState = (millis() / 500) % 2 == 0;
        if (blinkState != s_lastBlinkState) {
            s_lastBlinkState = blinkState;
            s_dirty = true;
        }
    }

    if (!s_dirty) return;

    s_display.clearDisplay();

    if (snap.stage == pet::LifeStage::HATCHLING) {
        renderHatchling(snap);
    } else {
        bool blinkHide = critical && s_lastBlinkState;
        renderPet(snap, blinkHide);
    }

    s_display.display();
    s_dirty = false;
}

} // namespace renderer
