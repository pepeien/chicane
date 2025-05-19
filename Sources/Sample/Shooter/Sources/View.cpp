#include "View.hpp"

#include "Game.hpp"

View::View()
    : Chicane::Grid::View("Contents/Shooter/Views/Home.grid"),
    m_didPlayerWin(true),
    m_telemetry(Chicane::Application::getTelemetry()),
    m_uiFrameTime(Chicane::Reference::fromValue<const float>(&m_telemetry.delta)),
    m_uiFramesPerSecond(Chicane::Reference::fromValue<const std::uint32_t>(&m_telemetry.rate)),
    m_victoryVisibility(Chicane::Grid::Style::DISPLAY_TYPE_HIDDEN),
    m_uiVictoryVisibility(Chicane::Reference::fromValue<std::string>(&m_victoryVisibility)),
    m_crosshairDotVisibility(Chicane::Grid::Style::DISPLAY_TYPE_HIDDEN),
    m_uiCrosshairDotVisibility(Chicane::Reference::fromValue<std::string>(&m_crosshairDotVisibility)),
    m_crosshairSize(1),
    m_uiCrosshairSize(Chicane::Reference::fromValue<int>(&m_crosshairSize)),
    m_crosshairThickness(0.25f),
    m_uiCrosshairThickness(Chicane::Reference::fromValue<float>(&m_crosshairThickness)),
    m_crosshairGap(0),
    m_uiCrosshairGap(Chicane::Reference::fromValue<int>(&m_crosshairGap)),
    m_crosshairColorR(255),
    m_uiCrosshairColorR(Chicane::Reference::fromValue<int>(&m_crosshairColorR)),
    m_crosshairColorG(255),
    m_uiCrosshairColorG(Chicane::Reference::fromValue<int>(&m_crosshairColorG)),
    m_crosshairColorB(255),
    m_uiCrosshairColorB(Chicane::Reference::fromValue<int>(&m_crosshairColorB)),
    m_crosshairColorA(255),
    m_uiCrosshairColorA(Chicane::Reference::fromValue<int>(&m_crosshairColorA))
{
    Game::watchScore(
        [this](std::uint32_t inScore)
        {
            if (Game::didReachMaxScore())
            {
                m_didPlayerWin = true;

                m_victoryVisibility = Chicane::Grid::Style::DISPLAY_TYPE_FLEX;
            }
        }
    );

    addReference("frameTime",           &m_uiFrameTime);
    addReference("freamesPerSecond",    &m_uiFramesPerSecond);
    addReference("victoryDisplay",      &m_uiVictoryVisibility);
    addReference("crosshairDotDisplay", &m_uiCrosshairDotVisibility);
    addReference("crosshairSize",       &m_uiCrosshairSize);
    addReference("crosshairThickness",  &m_uiCrosshairThickness);
    addReference("crosshairGap",        &m_uiCrosshairGap);
    addReference("crosshairColorR",     &m_uiCrosshairColorR);
    addReference("crosshairColorG",     &m_uiCrosshairColorG);
    addReference("crosshairColorB",     &m_uiCrosshairColorB);
    addReference("crosshairColorA",     &m_uiCrosshairColorA);
}