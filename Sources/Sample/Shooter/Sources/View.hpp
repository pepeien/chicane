#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Runtime/Grid.hpp"

class View : public Chicane::Grid::View
{
public:
    View();

private:
    Chicane::Grid::Reference getFPS(const Chicane::Grid::Event& inEvent);
    Chicane::Grid::Reference getFrametime(const Chicane::Grid::Event& inEvent);

private:
    bool                     m_didPlayerWin;
    Chicane::Grid::Reference m_uiDidPlayerWin;
};