#pragma once

#include <vector>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

namespace Editor
{
    CH_TYPE(Type = (Manual))
    struct OutlinerNode
    {
    public:
        using List = std::vector<OutlinerNode>;

    public:
        OutlinerNode();

    public:
        CH_FIELD()
        Chicane::Object* item;

        CH_FIELD()
        Chicane::String label;

        CH_FIELD()
        Chicane::String icon;

        CH_FIELD()
        Chicane::String indent;

        CH_FIELD()
        Chicane::String expandState;

        CH_FIELD()
        Chicane::String selectedState;

        CH_FIELD()
        bool bHasChildren;

        CH_FIELD()
        bool bIsLeaf;

        CH_FIELD()
        bool bIsEditing;

        CH_FIELD()
        bool bShowLabel;
    };
}
