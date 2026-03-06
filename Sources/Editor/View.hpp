#pragma once

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Runtime/Application/Telemetry.hpp>

namespace Editor
{
    CH_TYPE()
    class View : public Chicane::Grid::View
    {
    public:
        View();

    public:
        Chicane::Reference onAssetImport(const Chicane::Grid::Event& inEvent);

    public:
        CH_FIELD()
        const Chicane::ApplicationTelemetry& telemetry;
    };
}