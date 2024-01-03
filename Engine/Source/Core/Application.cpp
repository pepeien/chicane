#include "Application.hpp"

namespace Chicane
{
    Application::Application(
        const std::string& inWindowTitle,
        const Level::Instance& inLevel
    )
    : m_renderer(
        std::make_unique<Renderer::Application>(
            inWindowTitle,
            inLevel
        )
    )
    {}

    void Application::run()
    {
        m_renderer->run();
    }
}