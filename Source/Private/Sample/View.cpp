#include "View.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

#include "Actor/Apple.hpp"

View::View()
    : Chicane::Grid::View(
        "home",
        "Content/Sample/Views/Home.grid"
    ),
    m_newAppleLocation({})
{
    addVariable(
        "newAppleX",
        &m_newAppleLocation.x
    );
    addVariable(
        "newAppleY",
        &m_newAppleLocation.y
    );
    addVariable(
        "newAppleZ",
        &m_newAppleLocation.z
    );

    addFunction(
        "getFPS",
        std::bind(&View::getFPS, this, std::placeholders::_1)
    );
    addFunction(
        "getFrametime",
        std::bind(&View::getFrametime, this, std::placeholders::_1)
    );
    addFunction(
        "addNewApple",
        std::bind(&View::addNewApple, this, std::placeholders::_1)
    );
}


std::uint64_t View::getFPS(const Chicane::Grid::ComponentEvent& inEvent)
{
    return Chicane::getTelemetry().frame.rate;
}

float View::getFrametime(const Chicane::Grid::ComponentEvent& inEvent)
{
    return Chicane::getTelemetry().frame.time;
}

int View::addNewApple(const Chicane::Grid::ComponentEvent& inEvent)
{
    std::string x = Chicane::Utils::trim(std::any_cast<std::string>(m_newAppleLocation.x));
    std::string y = Chicane::Utils::trim(std::any_cast<std::string>(m_newAppleLocation.y));
    std::string z = Chicane::Utils::trim(std::any_cast<std::string>(m_newAppleLocation.z));

    Apple* newApple = new Apple();
    newApple->setAbsoluteTranslation(
        Chicane::Vec<float>::Three(
            x.empty() ? 0.0f : std::stof(x),
            x.empty() ? 0.0f : std::stof(y),
            x.empty() ? 0.0f : std::stof(z)
        )
    );

    Chicane::addActor(newApple);

    return 0;
}