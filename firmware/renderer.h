#pragma once
#include "pet.h"

namespace renderer {

void init();
void splash(const char* msg, uint16_t durationMs);
void frame(const pet::Snapshot& snap);
void markDirty();

} // namespace renderer
