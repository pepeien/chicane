#pragma once

#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Runtime/Application/Telemetry.hpp>

class View : public Chicane::Grid::View
{
public:
    View();

private:
    // Telemetry
    const Chicane::ApplicationTelemetry& m_telemetry;

    // Game
    bool m_bDidPlayerWin;

    // Camera
    Chicane::String m_currentCamera;

    // Crosshair
    Chicane::String m_crosshairDotVisibility;
    float           m_crosshairSize;
    float           m_crosshairThickness;
    float           m_crosshairGap;
    std::uint8_t    m_crosshairColorR;
    std::uint8_t    m_crosshairColorG;
    std::uint8_t    m_crosshairColorB;
    float           m_crosshairColorA;

    // Score
    std::uint32_t m_playerScore;
    std::uint32_t m_maxScore;

private:
    // Game
    Chicane::Reference u_DidPlayerWin;

    // Telemetry
    Chicane::Reference u_rendererFrameTime;
    Chicane::Reference u_rendererFramesPerSecond;

    Chicane::Reference u_sceneFrameTime;
    Chicane::Reference u_sceneFramesPerSecond;

    Chicane::Reference u_uiFrameTime;
    Chicane::Reference u_uiFramesPerSecond;

    // Camera
    Chicane::Reference u_currentCamera;

    // Crosshair
    Chicane::Reference u_crosshairDotVisibility;
    Chicane::Reference u_crosshairSize;
    Chicane::Reference u_crosshairThickness;
    Chicane::Reference u_crosshairGap;
    Chicane::Reference u_crosshairColorR;
    Chicane::Reference u_crosshairColorG;
    Chicane::Reference u_crosshairColorB;
    Chicane::Reference u_crosshairColorA;

    // Score
    Chicane::Reference u_playerScore;
    Chicane::Reference u_maxScore;
};