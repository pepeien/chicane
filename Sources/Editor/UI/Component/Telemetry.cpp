#include "Editor/UI/Component/Telemetry.reflected.hpp"

#include <Chicane/Runtime/Application.hpp>

namespace Editor
{
    Telemetry::Telemetry(const pugi::xml_node& inNode)
        : Chicane::Grid::Container(inNode),
          telemetry(&Chicane::Application::getInstance().getTelemetry())
    {
        load("Assets/Editor/UI/Components/Telemetry.grid", "Assets/Editor/UI/Components/Telemetry.decal");
    }
}
