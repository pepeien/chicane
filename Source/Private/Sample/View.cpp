#include "View.hpp"

#include "Chicane/Game.hpp"

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


std::uint64_t View::getFPS(Chicane::Grid::ComponentEvent inEvent)
{
    return Chicane::State::getTelemetry().frame.rate;
}

float View::getFrametime(Chicane::Grid::ComponentEvent inEvent)
{
    return Chicane::State::getTelemetry().frame.time;
}