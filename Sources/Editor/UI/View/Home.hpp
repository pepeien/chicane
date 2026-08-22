#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Runtime/Scene/Actor.hpp>

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
        void onThemeSwitch();

    public:
        CH_FIELD()
        std::vector<Chicane::Actor*> outlinerActors;

        CH_FIELD()
        Chicane::String theme;
    };
}
