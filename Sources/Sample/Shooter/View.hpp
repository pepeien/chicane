#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Runtime/Application/Telemetry.hpp>

CH_TYPE(Manual)
class View : public Chicane::Grid::View
{
public:
    View();

public:
    // Telemetry
    CH_FIELD()
    const Chicane::ApplicationTelemetry* telemetry;

    // Game
    CH_FIELD()
    bool bDidPlayerWin;

    // Camera
    CH_FIELD()
    Chicane::String currentCamera;

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
};