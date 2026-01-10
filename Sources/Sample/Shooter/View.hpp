#pragma once

#include "Chicane/Core/Telemetry.hpp"
#include "Chicane/Grid/Component/View.hpp"

class View : public Chicane::Grid::View
{
public:
    View();

private:
    bool m_didPlayerWin;

    // Status
    const Chicane::Telemetry& m_telemetry;

    // Telemetry
    Chicane::Reference m_uiFrameTime;
    Chicane::Reference m_uiFramesPerSecond;

    // Victory Banner
    Chicane::String    m_victoryVisibility;
    Chicane::Reference m_uiVictoryVisibility;

    // Camera
    Chicane::String    m_currentCamera;
    Chicane::Reference m_uiCurrentCamera;

    // Crosshair
    Chicane::String    m_crosshairDotVisibility;
    Chicane::Reference m_uiCrosshairDotVisibility;
    float              m_crosshairSize;
    Chicane::Reference m_uiCrosshairSize;
    float              m_crosshairThickness;
    Chicane::Reference m_uiCrosshairThickness;
    float              m_crosshairGap;
    Chicane::Reference m_uiCrosshairGap;
    std::uint8_t       m_crosshairColorR;
    Chicane::Reference m_uiCrosshairColorR;
    std::uint8_t       m_crosshairColorG;
    Chicane::Reference m_uiCrosshairColorG;
    std::uint8_t       m_crosshairColorB;
    Chicane::Reference m_uiCrosshairColorB;
    std::uint8_t       m_crosshairColorA;
    Chicane::Reference m_uiCrosshairColorA;

    // Score
    std::uint32_t      m_playerScore;
    Chicane::Reference m_uiPlayerScore;
};