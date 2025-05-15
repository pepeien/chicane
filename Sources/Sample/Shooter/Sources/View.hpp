#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid.hpp"

class View : public Chicane::Grid::View
{
public:
    View();

private:
    Chicane::Grid::Reference getFPS(const Chicane::Grid::Event& inEvent);
    Chicane::Grid::Reference getFrametime(const Chicane::Grid::Event& inEvent);

private:
    bool                     m_didPlayerWin;

    // Victory Banner
    std::string              m_victoryVisibility;
    Chicane::Grid::Reference m_uiVictoryVisibility;

    // Crosshair
    std::string              m_crosshairDotVisibility;
    Chicane::Grid::Reference m_uiCrosshairDotVisibility;
    int                      m_crosshairSize;
    Chicane::Grid::Reference m_uiCrosshairSize;
    float                    m_crosshairThickness;
    Chicane::Grid::Reference m_uiCrosshairThickness;
    int                      m_crosshairGap;
    Chicane::Grid::Reference m_uiCrosshairGap;
    int                      m_crosshairColorR;
    Chicane::Grid::Reference m_uiCrosshairColorR;
    int                      m_crosshairColorG;
    Chicane::Grid::Reference m_uiCrosshairColorG;
    int                      m_crosshairColorB;
    Chicane::Grid::Reference m_uiCrosshairColorB;
    int                      m_crosshairColorA;
    Chicane::Grid::Reference m_uiCrosshairColorA;
};