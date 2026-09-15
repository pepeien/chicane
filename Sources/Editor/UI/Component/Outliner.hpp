#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component/Container.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

#include "Editor/UI/Component/Outliner/Node.hpp"

namespace Editor
{
    CH_TYPE(Type = (Manual), Alias = (Editor::Outliner))
    class Outliner : public Chicane::Grid::Container
    {
    public:
        static constexpr inline const char* NODES_ATTRIBUTE             = "outlinerNodes";
        static constexpr inline const char* ON_ITEM_SELECTION_ATTRIBUTE = "onItemSelection";
        static constexpr inline const char* ON_ITEM_TOGGLE_ATTRIBUTE    = "onItemToggle";

    public:
        CH_CONSTRUCTOR()
        Outliner(const pugi::xml_node& inNode);

    protected:
        void onTick(float inDeltaTime) override;

    public:
        CH_FUNCTION()
        void onItemSelection(Chicane::Object* inItem);

        CH_FUNCTION()
        void onItemToggle(Chicane::Object* inItem);

    public:
        CH_FIELD()
        OutlinerNode::List outlinerNodes;
    };
}
