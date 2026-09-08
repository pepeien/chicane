#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Grid/Component/View.hpp>

#include "Sample/Shooter/UI/Component/Crosshair/Setting.hpp"

CH_TYPE(Type = (Manual), Alias = (Sample::HomeView))
class HomeView : public Chicane::Grid::View
{
public:
    HomeView();

public:
    // Game
    CH_FIELD()
    bool bDidPlayerWin;

    // Score
    CH_FIELD()
    std::uint32_t playerScore;
    CH_FIELD()
    std::uint32_t maxScore;

    // Input
    CH_FIELD()
    Chicane::String input;

    // Crosshair
    CH_FIELD()
    CrosshairSetting crosshair;
};
