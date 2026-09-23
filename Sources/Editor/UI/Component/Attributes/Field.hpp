#pragma once

#include <vector>

#include <Chicane/Core/Math/Vec/Vec3.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>

#include "Editor/UI/Component/Attributes/Field/Type.hpp"

namespace Editor
{
    CH_TYPE(Type = (Manual))
    struct AttributeField
    {
    public:
        using List = std::vector<AttributeField>;

    public:
        AttributeField();

    public:
        CH_FIELD()
        bool bIsChecked;

        CH_FIELD()
        Chicane::String name;
        CH_FIELD()
        Chicane::String label;
        CH_FIELD()
        Chicane::String group;
        CH_FIELD()
        Chicane::String description;
        CH_FIELD()
        AttributeFieldType type;
        CH_FIELD()
        Chicane::String kind;
        CH_FIELD()
        Chicane::String text;
        CH_FIELD()
        Chicane::Vec3 vector;
        CH_FIELD()
        std::vector<Chicane::String> options;
    };
}
