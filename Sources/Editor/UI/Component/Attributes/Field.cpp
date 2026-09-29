#include "Editor/UI/Component/Attributes/Field.reflected.hpp"

namespace Editor
{
    AttributeField::AttributeField()
        : bIsChecked(false),
          name(Chicane::String::sEmpty()),
          label(Chicane::String::sEmpty()),
          group(Chicane::String::sEmpty()),
          description(Chicane::String::sEmpty()),
          type(AttributeFieldType::Text),
          kind(Chicane::String::sEmpty()),
          text(Chicane::String::sEmpty()),
          vector(Chicane::Vec3::sZero()),
          options({})
    {}
}
