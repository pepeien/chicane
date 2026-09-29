#include "Editor/UI/Component/Telemetry.reflected.hpp"

#include <Chicane/Runtime/Instance.hpp>

namespace Editor
{
    Telemetry::Telemetry(const Chicane::XmlNode& inNode)
        : Chicane::Grid::Container(inNode),
          telemetry(&Chicane::Instance::sInstance().getTelemetry())
    {
        load("Assets/Editor/UI/Components/Telemetry/Index.grid", "Assets/Editor/UI/Components/Telemetry/Index.decal");
    }
}
