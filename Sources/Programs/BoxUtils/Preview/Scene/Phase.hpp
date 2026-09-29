#pragma once

#include <cstdint>

enum class PreviewScenePhase : std::uint8_t
{
    Load,
    Warmup,
    Done
};
