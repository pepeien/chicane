#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid.hpp"

class View : public Chicane::Grid::View
{
public:
    View();

private:
    Chicane::Grid::Reference getFPS(const Chicane::Grid::Component::Event& inEvent);
    Chicane::Grid::Reference getFrametime(const Chicane::Grid::Component::Event& inEvent);

private:
    void loadAudio(const std::string& inFilepath);
    void playAudio(const std::string& inId);

private:
    bool                     m_didPlayerWin;
    Chicane::Grid::Reference m_uiDidPlayerWin;
};