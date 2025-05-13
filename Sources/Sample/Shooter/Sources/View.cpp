#include "View.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Application.hpp"

#include "Actor/Apple.hpp"
#include "Game.hpp"

View::View()
    : Chicane::Grid::View(
        "home",
        "Contents/Shooter/Views/Home.grid"
    ),
    m_didPlayerWin(true),
    m_victoryVisibility(Chicane::Grid::Style::DISPLAY_TYPE_HIDDEN),
    m_uiVictoryVisibility(Chicane::Grid::Reference::fromValue<std::string>(&m_victoryVisibility)),
    m_crosshairSize(1.0f),
    m_uiCrosshairSize(Chicane::Grid::Reference::fromValue<float>(&m_crosshairSize)),
    m_crosshairThickness(0.25f),
    m_uiCrosshairThickness(Chicane::Grid::Reference::fromValue<float>(&m_crosshairThickness)),
    m_crosshairGap(1.0f),
    m_uiCrosshairGap(Chicane::Grid::Reference::fromValue<float>(&m_crosshairGap)),
    m_crosshairColorR(255),
    m_uiCrosshairColorR(Chicane::Grid::Reference::fromValue<int>(&m_crosshairColorR)),
    m_crosshairColorG(255),
    m_uiCrosshairColorG(Chicane::Grid::Reference::fromValue<int>(&m_crosshairColorG)),
    m_crosshairColorB(255),
    m_uiCrosshairColorB(Chicane::Grid::Reference::fromValue<int>(&m_crosshairColorB)),
    m_crosshairColorA(255),
    m_uiCrosshairColorA(Chicane::Grid::Reference::fromValue<int>(&m_crosshairColorA))
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

    addReference(
        "victoryDisplay",
        &m_uiVictoryVisibility
    );
    addReference(
        "crosshairSize",
        &m_uiCrosshairSize
    );
    addReference(
        "crosshairThickness",
        &m_uiCrosshairThickness
    );
    addReference(
        "crosshairGap",
        &m_uiCrosshairGap
    );
    addReference(
        "crosshairColorR",
        &m_uiCrosshairColorR
    );
    addReference(
        "crosshairColorG",
        &m_uiCrosshairColorG
    );
    addReference(
        "crosshairColorB",
        &m_uiCrosshairColorB
    );
    addReference(
        "crosshairColorA",
        &m_uiCrosshairColorA
    );

    addFunction(
        "getFPS",
        std::bind(&View::getFPS, this, std::placeholders::_1)
    );
    addFunction(
        "getFrametime",
        std::bind(&View::getFrametime, this, std::placeholders::_1)
    );
}

Chicane::Grid::Reference View::getFPS(const Chicane::Grid::Event& inEvent)
{
    return Chicane::Grid::Reference::fromValue<const std::uint32_t>(
        &Chicane::Application::getTelemetry().frame.rate
    );
}

Chicane::Grid::Reference View::getFrametime(const Chicane::Grid::Event& inEvent)
{
    return Chicane::Grid::Reference::fromValue<const float>(
        &Chicane::Application::getTelemetry().frame.delta
    );
}