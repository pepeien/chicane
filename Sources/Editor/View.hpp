#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Runtime/Application/Telemetry.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    class View : public Chicane::Grid::View
    {
    public:
        View();

    public:
        CH_FUNCTION()
        void onAssetImport();

    public:
        CH_FIELD()
        const Chicane::ApplicationTelemetry* telemetry;
    };
}