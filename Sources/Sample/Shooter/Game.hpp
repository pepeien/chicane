#pragma once

#include <cstdint>
#include <functional>

#include "Sample/Export.hpp"

namespace Game
{
    CHICANE_SAMPLE_API void boot();

    std::uint32_t getScore();
    void incrementScore(std::uint32_t inScore);
    void decrementScore(std::uint32_t inScore);

    void watchScore(std::function<void(std::uint32_t)> inNext);

    std::uint32_t getMaxScore();
    bool didReachMaxScore();
    void setMaxScore(std::uint32_t inScore);
}