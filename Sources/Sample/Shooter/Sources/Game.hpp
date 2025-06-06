#pragma once

#include "Essential.hpp"

namespace Game
{
    std::uint32_t getScore();
    void incrementScore(std::uint32_t inScore);
    void decrementScore(std::uint32_t inScore);

    void watchScore(std::function<void (std::uint32_t)> inNext);

    bool didReachMaxScore();
    void setMaxScore(std::uint32_t inScore);
}