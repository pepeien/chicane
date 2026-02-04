#pragma once

#include <Chicane/Core/Telemetry.hpp>
#include <Chicane/Grid/Component/View.hpp>

namespace Editor
{
    class View : public Chicane::Grid::View
    {
    public:
        View();

    private:
        // Status
        const Chicane::Telemetry& m_telemetry;

        // Telemetry
        Chicane::Reference m_uiFrameTime;
        Chicane::Reference m_uiFramesPerSecond;
    };
}