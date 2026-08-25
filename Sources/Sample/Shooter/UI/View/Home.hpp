#pragma once

#include <cstdint>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Grid/Component/View.hpp>

CH_TYPE(Manual)
class HomeView : public Chicane::Grid::View
{
public:
    HomeView();

public:
    // Game
    CH_FIELD()
    bool bDidPlayerWin;

    // Crosshair
    CH_FIELD()
    Chicane::String crosshairDotVisibility;
    CH_FIELD()
    float crosshairSize;
    CH_FIELD()
    float crosshairThickness;
    CH_FIELD()
    float crosshairGap;
    CH_FIELD()
    std::uint8_t crosshairColorR;
    CH_FIELD()
    std::uint8_t crosshairColorG;
    CH_FIELD()
    std::uint8_t crosshairColorB;
    CH_FIELD()
    float crosshairColorA;

    // Score
    CH_FIELD()
    std::uint32_t playerScore;
    CH_FIELD()
    std::uint32_t maxScore;

    // Input
    CH_FIELD()
    Chicane::String input;
};
