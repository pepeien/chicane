#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component/View.hpp>

#include "Sample/Shooter/UI/Component/Crosshair/Setting.hpp"

CH_TYPE(Type = (Manual), Alias = (Sample::HomeView))
class HomeView : public Chicane::Grid::View
{
public:
    HomeView();

public:
    CH_FIELD()
    CrosshairSetting crosshair;
};
