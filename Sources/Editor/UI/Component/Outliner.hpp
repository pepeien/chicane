#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class Outliner : public Chicane::Grid::Component
    {
    public:
        CH_CONSTRUCTOR()
        Outliner(const pugi::xml_node& inNode);

    public:
        CH_FUNCTION()
        void onItemSelection(Chicane::Object* inItem);

    public:
        CH_FIELD()
        bool bIsItemSelected;

        CH_FIELD()
        Chicane::Object* selectedItem;
    };
}
