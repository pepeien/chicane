#pragma once

#include "Essential.hpp"

class View : public Chicane::Grid::View
{
public:
    View();

private:
    bool                      m_didPlayerWin;

    // Status
    const Chicane::Telemetry& m_telemetry;

    // Telemetry
    Chicane::Reference        m_uiFrameTime;
    Chicane::Reference        m_uiFramesPerSecond;

    // Victory Banner
    Chicane::String           m_victoryVisibility;
    Chicane::Reference        m_uiVictoryVisibility;

    // Crosshair
    Chicane::String           m_crosshairDotVisibility;
    Chicane::Reference        m_uiCrosshairDotVisibility;
    float                     m_crosshairSize;
    Chicane::Reference        m_uiCrosshairSize;
    float                     m_crosshairThickness;
    Chicane::Reference        m_uiCrosshairThickness;
    float                     m_crosshairGap;
    Chicane::Reference        m_uiCrosshairGap;
    std::uint8_t              m_crosshairColorR;
    Chicane::Reference        m_uiCrosshairColorR;
    std::uint8_t              m_crosshairColorG;
    Chicane::Reference        m_uiCrosshairColorG;
    std::uint8_t              m_crosshairColorB;
    Chicane::Reference        m_uiCrosshairColorB;
    std::uint8_t              m_crosshairColorA;
    Chicane::Reference        m_uiCrosshairColorA;
};