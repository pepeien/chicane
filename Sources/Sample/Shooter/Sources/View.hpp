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

    // Crosshair
    float                    m_crosshairSize;
    Chicane::Grid::Reference m_uiCrosshairSize;
    float                    m_crosshairThickness;
    Chicane::Grid::Reference m_uiCrosshairThickness;
    float                    m_crosshairGap;
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