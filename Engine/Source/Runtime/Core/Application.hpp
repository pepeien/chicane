#pragma once

#include "Renderer/Application.hpp"

namespace Chicane
{
    class Application
    {
    public:
        Application(
            const std::string& inWindowTitle,
            const Level::Instance& inLevel
        );
    
    public:
        void run();
    
    private:
        std::unique_ptr<Renderer::Application> m_renderer;
    };
}