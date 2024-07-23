#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid.hpp"

class View : public Chicane::Grid::View
{
public:
    View();

private:
    std::uint64_t getFPS(Chicane::Grid::ComponentEvent inEvent);
    float getFrametime(Chicane::Grid::ComponentEvent inEvent);
};