# Boing — Firmware Architecture

The original firmware was a 256-line monolithic `.ino` file.
This redesign splits it into eight modules with clear ownership boundaries,
replaces the polling FSM with an event-driven input model, and introduces a
pet life-stage system and exponential stat decay.

---

## Module map

```
firmware/
├── firmware.ino     Entry point — wires modules together, owns alert timer
├── config.h         All hardware pins and tuning constants (no .cpp)
├── pet.h / .cpp     Pet model: vitals, decay, life stage, mood
├── input.h / .cpp   Button GPIO with debounce; fires callbacks
├── audio.h / .cpp   Non-blocking melody sequencer
├── renderer.h / .cpp  Display abstraction with dirty-flag and blink
├── sprites.h / .cpp   PROGMEM bitmaps + [stage][mood] lookup table
```

Dependency graph (no cycles):

```
firmware.ino ──► pet, input, audio, renderer
renderer     ──► pet (Snapshot type), sprites, config
pet          ──► config
input        ──► config
audio        ──► config
sprites      ──► (none)
```

---

## Key design decisions

### 1. Event-driven input instead of global FSM

**Original:** `checkButtons()` mutated a global `Screen currentScreen` enum,
then `handleScreenLogic()` switched on it to apply stat changes, then reset it
to `SCREEN_MAIN`.

**New:** `input::onPress()` registers a callback. `input::poll()` fires it with
the `Button` that was pressed. `firmware.ino` registers `onButton()`, which calls
`pet::feed()` / `pet::play()` / `pet::rest()` directly. There is no screen-state
variable at all.

Benefit: input and game logic are decoupled. Adding a new action (e.g., "pet the
pet") requires only a new `pet::` method and one extra `case` in `onButton()` —
no FSM states to add or update.

### 2. Float vitals and exponential decay

**Original:** `int hunger/happiness/energy` in range 0–100, decremented by a
fixed `–1` every 5 seconds.

**New:** `float hunger/joy/energy` in range 0.0–1.0. Decay per tick:

```cpp
delta = DECAY_BASE * exp(DECAY_EXPONENT * (1.0 - vital))
```

At full saturation (1.0) the delta equals `DECAY_BASE` (~1% / 5 s), matching the
original rate. As a stat falls toward zero the decay accelerates — at 0.0 it is
`exp(1.5) ≈ 4.5×` faster. This creates escalating urgency without requiring any
additional game logic.

Floats are cheap on the ESP32-C6 (hardware FPU); the extra precision also means
future effects (partial actions, aging penalties) can be tuned without rescaling
the entire model.

### 3. Life-stage progression

**Original:** `pet.age` tracked elapsed seconds with no gameplay effect.

**New:** four stages gate different sprites and behaviours:

| Stage      | Age range     | Vitals decay | Sprite family  |
|------------|---------------|--------------|----------------|
| HATCHLING  | 0 – 5 min     | none         | egg            |
| JUVENILE   | 5 – 20 min    | normal       | round blob     |
| ADULT      | 20 – 60 min   | normal       | eared + feet   |
| ELDER      | 60 min+       | normal       | adult + "zzz"  |

During HATCHLING, the display shows the egg sprite and a hatching progress bar
instead of stat bars. Vitals decay is suppressed — the pet is protected in the egg.

### 4. Sprite lookup table instead of if-else chain

**Original:**
```cpp
if      (hunger < 30 || ...) sprite = petSad;
else if (hunger > 50 && ...) sprite = petHappy;
else                          sprite = petNeutral;
```

**New:** `Mood` is derived once in `pet::moodFor()` from a weighted wellness
score, then the renderer indexes into a 2-D table:

```cpp
const uint8_t* spr = SPRITE_TABLE[stageIdx][moodIdx];
```

Adding a new stage or mood means adding one row/column to the table and the
corresponding PROGMEM bitmap. No conditional logic to touch.

### 5. Wellness score as a composite metric

Mood is determined by a single float:

```cpp
wellness = hunger × 0.40 + joy × 0.35 + energy × 0.25
```

`HAPPY` when wellness ≥ 0.75, `NEUTRAL` ≥ 0.45, `SAD` below that.
Energy < 0.15 forces `SLEEPING` regardless of other stats.

The weights can be adjusted in `config.h` to shift which stat dominates mood
without touching conditional logic.

### 6. Non-blocking audio sequencer

**Original:** `tone(BUZZER_PIN, 1000, 50)` — a single blocking-duration note.

**New:** `audio::play(Sound)` queues a `Melody` (array of `{freq, durationMs}`
pairs). `audio::tick()` advances the melody on each `loop()` without blocking.
Silence gaps are represented as `{freq: 0}` notes. The ALERT melody uses a gap:

```
523 Hz — 200 ms — silence 100 ms — 523 Hz 200 ms
```

### 7. Dirty-flag rendering with critical blink

**Original:** Every frame called `display.clearDisplay()` + redrew everything +
called `display.display()` (a full I²C frame transfer, ~1 ms).

**New:** `renderer::markDirty()` is called by the `pet::onChanged` callback.
`renderer::frame()` skips `display.display()` unless `s_dirty` is set.

When any vital drops below `CRITICAL_THRESHOLD` (0.20), the renderer
automatically toggles `s_dirty` every 500 ms to produce a sprite blink —
handled entirely inside the renderer without any external timer.

### 8. Layout constants namespace

All pixel coordinates live in `renderer::Layout`. The original had `24`, `34`,
`44`, `56`, `100`, `6` scattered across `drawBar()` and `render()` with no
labels. Changing the layout required hunting for magic numbers.

---

## Porting / customization guide

**Change decay speed:** edit `Tuning::DECAY_BASE` and `Tuning::DECAY_EXPONENT`
in `config.h`.

**Change action effects:** edit the `FEED_DELTA` / `PLAY_*` / `REST_DELTA`
constants in `config.h`.

**Change life-stage thresholds:** edit `JUVENILE_AGE_S` / `ADULT_AGE_S` /
`ELDER_AGE_S` (in seconds).

**Add a new action:** add a `pet::` method in `pet.h` / `pet.cpp`, then a new
`case` in `onButton()` in `firmware.ino`.

**Add a new sprite:** add a `PROGMEM` array in `sprites.cpp`, declare it `extern`
in `sprites.h`, and update `SPRITE_TABLE`.

**Change button mapping:** edit `HW::BTN_A/B/C` in `config.h`.
