#pragma once

#include "Chicane/Core.hpp"

namespace Game
{
    std::uint32_t getScore();
    void setScore(std::uint32_t inScore);
    void incrementScore(std::uint32_t inScore);
    void decrementScore(std::uint32_t inScore);

    void watchScore(std::function<void (std::uint32_t)> inNext);

    bool didReachMaxScore();
    void setMaxScore(std::uint32_t inScore);
}