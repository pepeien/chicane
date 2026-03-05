#pragma once

#include <Chicane/Grid/Component/View.hpp>
#include <Chicane/Runtime/Application/Telemetry.hpp>

namespace Editor
{
    class View : public Chicane::Grid::View
    {
    public:
        View();

    private:
        Chicane::Reference onAssetImport(const Chicane::Grid::Event& inEvent);

    private:
        // Status
        const Chicane::ApplicationTelemetry& m_telemetry;

        // Telemetry
        Chicane::Reference u_sceneFrameTime;
        Chicane::Reference u_sceneFramesPerSecond;
    };
}