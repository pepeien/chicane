#pragma once

#include <Chicane/Core/FileSystem.hpp>
#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Runtime/Application/Telemetry.hpp>
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

    public:
        CH_FIELD()
        const Chicane::ApplicationTelemetry* telemetry;

        CH_FIELD()
        std::vector<Chicane::Actor*> actors;

        CH_FIELD()
        std::vector<Chicane::FileSystem::Item> folderItems;
    };
}