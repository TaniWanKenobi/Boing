#pragma once
#include <Arduino.h>

namespace pet {

// Ordered so that the integer value indexes SPRITE_TABLE rows/columns correctly.
enum class LifeStage : uint8_t { HATCHLING = 0, JUVENILE = 1, ADULT = 2, ELDER = 3 };
enum class Mood      : uint8_t { HAPPY = 0, NEUTRAL = 1, SAD = 2, SLEEPING = 3 };

struct Vitals {
    float hunger;   // [0, 1]
    float joy;      // [0, 1]
    float energy;   // [0, 1]
};

struct Snapshot {
    Vitals    vitals;
    LifeStage stage;
    Mood      mood;
    float     wellness; // weighted composite of vitals
    uint32_t  ageSecs;
};

using ChangeCallback = void(*)();

void     init();
void     tick(uint32_t nowMs);
void     feed();
void     play();
void     rest();
Snapshot snapshot();
void     onChanged(ChangeCallback cb);

} // namespace pet
