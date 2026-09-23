#include "Editor/UI/Component/Attributes/Field.reflected.hpp"

namespace Editor
{
    AttributeField::AttributeField()
        : bIsChecked(false),
          name(Chicane::String::empty()),
          label(Chicane::String::empty()),
          group(Chicane::String::empty()),
          description(Chicane::String::empty()),
          type(AttributeFieldType::Text),
          kind(Chicane::String::empty()),
          text(Chicane::String::empty()),
          vector(Chicane::Vec3::Zero()),
          options({})
    {}
}
