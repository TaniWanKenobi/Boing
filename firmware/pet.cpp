#include "pet.h"
#include "config.h"
#include <math.h>

namespace pet {

static Vitals         s_vitals;
static uint32_t       s_ageSecs   = 0;
static uint32_t       s_lastTick  = 0;
static ChangeCallback s_onChange  = nullptr;

static void clamp01(float& v) {
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
}

// Decay is fastest when a stat is already low — gives urgency to critical states.
static float decayDelta(float v) {
    return Tuning::DECAY_BASE * expf(Tuning::DECAY_EXPONENT * (1.0f - v));
}

static LifeStage stageFor(uint32_t age) {
    if (age < Tuning::JUVENILE_AGE_S) return LifeStage::HATCHLING;
    if (age < Tuning::ADULT_AGE_S)   return LifeStage::JUVENILE;
    if (age < Tuning::ELDER_AGE_S)   return LifeStage::ADULT;
    return LifeStage::ELDER;
}

static Mood moodFor(const Vitals& v) {
    if (v.energy < 0.15f) return Mood::SLEEPING;
    float w = v.hunger * Tuning::WELLNESS_HUNGER_W
            + v.joy    * Tuning::WELLNESS_JOY_W
            + v.energy * Tuning::WELLNESS_ENERGY_W;
    if (w >= 0.75f) return Mood::HAPPY;
    if (w >= 0.45f) return Mood::NEUTRAL;
    return Mood::SAD;
}

void init() {
    s_vitals    = { 0.80f, 0.80f, 0.80f };
    s_ageSecs   = 0;
    s_lastTick  = millis();
}

void tick(uint32_t nowMs) {
    if (nowMs - s_lastTick < Tuning::DECAY_INTERVAL_MS) return;
    s_lastTick  += Tuning::DECAY_INTERVAL_MS;
    s_ageSecs   += Tuning::DECAY_INTERVAL_MS / 1000;

    // Stats don't decay while still in the egg — pet hatches at full health.
    if (stageFor(s_ageSecs) != LifeStage::HATCHLING) {
        s_vitals.hunger -= decayDelta(s_vitals.hunger);
        s_vitals.joy    -= decayDelta(s_vitals.joy);
        s_vitals.energy -= decayDelta(s_vitals.energy);
        clamp01(s_vitals.hunger);
        clamp01(s_vitals.joy);
        clamp01(s_vitals.energy);
    }

    if (s_onChange) s_onChange();
}

void feed() {
    s_vitals.hunger += Tuning::FEED_DELTA;
    clamp01(s_vitals.hunger);
    if (s_onChange) s_onChange();
}

void play() {
    s_vitals.joy    += Tuning::PLAY_JOY_DELTA;
    s_vitals.energy -= Tuning::PLAY_ENERGY_COST;
    clamp01(s_vitals.joy);
    clamp01(s_vitals.energy);
    if (s_onChange) s_onChange();
}

void rest() {
    s_vitals.energy += Tuning::REST_DELTA;
    clamp01(s_vitals.energy);
    if (s_onChange) s_onChange();
}

Snapshot snapshot() {
    float w = s_vitals.hunger * Tuning::WELLNESS_HUNGER_W
            + s_vitals.joy    * Tuning::WELLNESS_JOY_W
            + s_vitals.energy * Tuning::WELLNESS_ENERGY_W;
    return { s_vitals, stageFor(s_ageSecs), moodFor(s_vitals), w, s_ageSecs };
}

void onChanged(ChangeCallback cb) { s_onChange = cb; }

} // namespace pet
