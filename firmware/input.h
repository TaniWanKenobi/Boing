#pragma once
#include <Arduino.h>

namespace input {

enum class Button : uint8_t { A, B, C };

using PressCallback = void(*)(Button);

void init();
void poll();
void onPress(PressCallback cb);

} // namespace input
