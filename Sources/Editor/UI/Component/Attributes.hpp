#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>
#include <Chicane/Grid/Component/Container.hpp>

#include "Editor/UI/Component/Attributes/Group.hpp"

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::Attributes))
    class Attributes : public Chicane::Grid::Container
    {
    public:
        static constexpr inline const char* ITEM_SELECTED_ATTRIBUTE       = "bIsItemSelected";
        static constexpr inline const char* GROUPS_ATTRIBUTE              = "attributeGroups";
        static constexpr inline const char* ON_ATTRIBUTE_COMMIT_ATTRIBUTE = "onAttributeCommit";

    public:
        CH_CONSTRUCTOR()
        Attributes(const Chicane::XmlNode& inNode);

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onAttributeCommit(Chicane::String inName, Chicane::String inValue);

        CH_FUNCTION()
        Chicane::String getFieldDescription(Chicane::String inName);

    public:
        CH_FIELD()
        bool bIsItemSelected;

        CH_FIELD()
        AttributeGroup::List attributeGroups;
    };
}
