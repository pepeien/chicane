#include "View.hpp"

#include <Chicane/Runtime/Application.hpp>

namespace Editor
{
    View::View()
        : Chicane::Grid::View("Contents/Editor/Views/Home.grid"),
          m_telemetry(Chicane::Application::getInstance().getTelemetry()),
          m_uiFrameTime(Chicane::Reference::fromValue<const float>(&m_telemetry.delta)),
          m_uiFramesPerSecond(Chicane::Reference::fromValue<const std::uint32_t>(&m_telemetry.rate))
    {
        addReference("frameTime", &m_uiFrameTime);
        addReference("framesPerSecond", &m_uiFramesPerSecond);
    }
}