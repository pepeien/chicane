#include "View.hpp"

#include "Actor/Apple.hpp"
#include "Game.hpp"

#include "Chicane/Application.hpp"
#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

View::View()
    : Chicane::Grid::View(
        "home",
        "Content/Sample/Views/Home.grid"
    ),
    m_didPlayerWin(false),
    m_uiDidPlayerWin(Chicane::Grid::Reference::fromValue<bool>(&m_didPlayerWin))
{
    loadAudio("Content/Sample/Sounds/Hit.baud");
    loadAudio("Content/Sample/Sounds/Victory.baud");

    Game::watchScore(
        [this](std::uint32_t inScore)
        {
            if (Game::didReachMaxScore())
            {
                playAudio("Content/Sample/Sounds/Victory.baud");

                m_didPlayerWin = true;
            }
            else
            {
                playAudio("Content/Sample/Sounds/Hit.baud");
            }
        }
    );

    addVariable(
        "didPlayerWin",
        &m_uiDidPlayerWin
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

Chicane::Grid::Reference View::getFPS(const Chicane::Grid::Component::Event& inEvent)
{
    return Chicane::Grid::Reference::fromValue<const std::uint32_t>(
        &Chicane::Application::getTelemetry().frame.rate
    );
}

Chicane::Grid::Reference View::getFrametime(const Chicane::Grid::Component::Event& inEvent)
{
    return Chicane::Grid::Reference::fromValue<const float>(
        &Chicane::Application::getTelemetry().frame.time
    );
}

void View::loadAudio(const std::string& inFilepath)
{
    Chicane::Loader::loadAudio(inFilepath);
}

void View::playAudio(const std::string& inId)
{
    Chicane::Loader::getAudioManager()->play(inId);
}