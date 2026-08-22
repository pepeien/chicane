#include "Sample/Shooter/UI/Component/Telemetry.reflected.hpp"

#include <Chicane/Runtime/Application.hpp>

Telemetry::Telemetry(const pugi::xml_node& inNode)
    : Chicane::Grid::Container(inNode),
      telemetry(&Chicane::Application::getInstance().getTelemetry())
{
    load("Assets/Sample/Shooter/UI/Components/Telemetry.grid", "Assets/Sample/Shooter/UI/Components/Telemetry.decal");
}
