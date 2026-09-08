#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/Xml.hpp>
#include <Chicane/Grid/Component/Container.hpp>

#include "Sample/Shooter/UI/Component/Crosshair/Setting.hpp"

CH_TYPE(Type = (Manual), Alias = (Sample::Crosshair))
class Crosshair : public Chicane::Grid::Container
{
public:
    static constexpr inline const char* SETTINGS_ATTRIBUTE_NAME = "settings";

public:
    CH_CONSTRUCTOR()
    Crosshair(const pugi::xml_node& inNode);

protected:
    void onTick(float inDeltaTime) override;

private:
    void refreshSettings();

public:
    CH_FIELD()
    CrosshairSetting settings;
};
