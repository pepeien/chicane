#include "Sample/Shooter/UI/Component/Crosshair/Setting.reflected.hpp"

#include <Chicane/Core/Color.hpp>
#include <Chicane/Grid/Style.hpp>

CrosshairSetting::CrosshairSetting()
    : size(2.0f),
      thickness(1.0f),
      gap(1.0f),
      color(Chicane::Color::HEX_COLOR_WHITE),
      dot(Chicane::Grid::Style::DISPLAY_TYPE_BLOCK)
{}
