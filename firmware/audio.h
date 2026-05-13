#pragma once
#include <Arduino.h>

namespace audio {

enum class Sound : uint8_t { FEED, PLAY, REST, ALERT, BOOT };

void init();
void play(Sound s);
void tick(); // advance non-blocking melody sequencer; call every loop()

} // namespace audio
