#pragma once

#include <vector>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>

#include "Editor/UI/Component/Attributes/Field.hpp"

namespace Editor
{
    CH_TYPE(Type = (Manual))
    struct AttributeGroup
    {
    public:
        using List = std::vector<AttributeGroup>;

    public:
        AttributeGroup();

    public:
        CH_FIELD()
        Chicane::String label;

        CH_FIELD()
        AttributeField::List fields;
    };
}
