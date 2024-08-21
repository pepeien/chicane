#include "View.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"

#include "Actor/Apple.hpp"

View::View()
    : Chicane::Grid::View(
        "home",
        "Content/Sample/Views/Home.grid"
    )
{
    addFunction(
        "getFPS",
        std::bind(&View::getFPS, this, std::placeholders::_1)
    );
    addFunction(
        "getFrametime",
        std::bind(&View::getFrametime, this, std::placeholders::_1)
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