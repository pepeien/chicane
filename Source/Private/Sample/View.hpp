#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid.hpp"

struct Location {
    std::any x = std::make_any<std::string>("0");
    std::any y = std::make_any<std::string>("0");
    std::any z = std::make_any<std::string>("0");
};

class View : public Chicane::Grid::View
{
public:
    View();

private:
    std::uint64_t getFPS(const Chicane::Grid::ComponentEvent& inEvent);
    float getFrametime(const Chicane::Grid::ComponentEvent& inEvent);
    int addNewApple(const Chicane::Grid::ComponentEvent& inEvent);

private:
    Location m_newAppleLocation;
};