#include "Sample/Shooter/UI/Component/Crosshair.reflected.hpp"

#include <Chicane/Grid.hpp>

Crosshair::Crosshair(const pugi::xml_node& inNode)
    : Chicane::Grid::Container(inNode),
      settings()
{
    load("Assets/Sample/Shooter/UI/Components/Crosshair.grid", "Assets/Sample/Shooter/UI/Components/Crosshair.decal");
    refreshSettings();
}

void Crosshair::onTick(float inDeltaTime)
{
    Chicane::Grid::Container::onTick(inDeltaTime);

    refreshSettings();
}

void Crosshair::refreshSettings()
{
    const Chicane::String raw = getAttribute(SETTINGS_ATTRIBUTE_NAME);
    if (raw.isEmpty() || !isReference(raw))
    {
        return;
    }

    const Chicane::String id =
        raw.getBetween(Chicane::Grid::REFERENCE_VALUE_OPENING, Chicane::Grid::REFERENCE_VALUE_CLOSING).trim();
    if (id.isEmpty())
    {
        return;
    }

    for (Chicane::Grid::Component* node = getParent(); node != nullptr; node = node->getParent())
    {
        const Chicane::ReflectionFieldAccessor accessor = node->getField(id);
        if (accessor.isValid() && accessor.isType<CrosshairSetting>())
        {
            void* instance = accessor.boundInstance != nullptr ? const_cast<void*>(accessor.boundInstance)
                                                               : static_cast<void*>(node);

            if (const CrosshairSetting* parsed = accessor.getValue<CrosshairSetting>(instance))
            {
                settings = *parsed;
            }

            return;
        }

        if (node->isRoot())
        {
            return;
        }
    }
}
