#pragma once
#include <Arduino.h>

static constexpr uint8_t SPR_W = 16;
static constexpr uint8_t SPR_H = 16;

extern const uint8_t spr_egg[];

extern const uint8_t spr_juv_happy[];
extern const uint8_t spr_juv_neutral[];
extern const uint8_t spr_juv_sad[];
extern const uint8_t spr_juv_sleep[];

extern const uint8_t spr_adu_happy[];
extern const uint8_t spr_adu_neutral[];
extern const uint8_t spr_adu_sad[];
extern const uint8_t spr_adu_sleep[];

// SPRITE_TABLE[LifeStage][Mood] — both enums ordered to match
extern const uint8_t* const SPRITE_TABLE[4][4];
