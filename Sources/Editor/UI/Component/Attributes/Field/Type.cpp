#include "Editor/UI/Component/Attributes/Field/Type.hpp"
#include "Editor/UI/Component/Attributes/Field/Type.reflected.hpp"

namespace Editor
{
    Chicane::String toString(AttributeFieldType inValue)
    {
        switch (inValue)
        {
        case AttributeFieldType::Text:
            return "Text";

        case AttributeFieldType::Bool:
            return "Bool";

        case AttributeFieldType::Enum:
            return "Enum";

        case AttributeFieldType::Vec3:
            return "Vec3";

        case AttributeFieldType::Float:
            return "Float";

        case AttributeFieldType::Color:
            return "Color";

        case AttributeFieldType::Asset:
            return "Asset";

        default:
            return "";
        }
    }
}
