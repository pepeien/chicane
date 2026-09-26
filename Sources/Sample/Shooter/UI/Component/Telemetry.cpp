#include "Sample/Shooter/UI/Component/Telemetry.reflected.hpp"

#include <Chicane/Runtime/Instance.hpp>

Telemetry::Telemetry(const Chicane::XmlNode& inNode)
    : Chicane::Grid::Container(inNode),
      telemetry(&Chicane::Instance::sInstance().getTelemetry())
{
    load(
        "Assets/Sample/Shooter/UI/Components/Telemetry/Index.grid",
        "Assets/Sample/Shooter/UI/Components/Telemetry/Index.decal"
    );
}
