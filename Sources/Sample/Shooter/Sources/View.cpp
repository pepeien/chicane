#include "View.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Application.hpp"

#include "Actor/Apple.hpp"
#include "Game.hpp"

View::View()
    : Chicane::Grid::View(
        "home",
        "Contents/Samples/Shooter/Views/Home.grid"
    ),
    m_didPlayerWin(false),
    m_uiDidPlayerWin(Chicane::Grid::Reference::fromValue<bool>(&m_didPlayerWin)),
    m_crosshairGap(0.0),
    m_uiCrosshairGap(Chicane::Grid::Reference::fromValue<float>(&m_crosshairGap))
{
    Game::watchScore(
        [this](std::uint32_t inScore)
        {
            if (Game::didReachMaxScore())
            {
                m_didPlayerWin = true;
            }
        }
    );

    addReference(
        "didPlayerWin",
        &m_uiDidPlayerWin
    );
    addReference(
        "crosshairGap",
        &m_uiCrosshairGap
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

void View::onTick(float inDelta)
{
    m_crosshairGap += 0.0001f;
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