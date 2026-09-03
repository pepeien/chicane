#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>
#include <Chicane/Runtime/Scene/Object.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class HomeView : public Chicane::Grid::View
    {
    public:
        HomeView();

    public:
        CH_FUNCTION()
        void onAssetImport();

        CH_FUNCTION()
        void onThemeSwitch(Chicane::String inValue);

        CH_FUNCTION()
        void onItemSelection(Chicane::Object* inItem);

    public:
        CH_FIELD()
        std::vector<Chicane::Actor*> outlinerActors;

        CH_FIELD()
        bool bIsItemSelected;

        CH_FIELD()
        Chicane::Object* selectedItem;

        CH_FIELD()
        Chicane::String theme;
    };
}
