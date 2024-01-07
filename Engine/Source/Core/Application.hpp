#pragma once

#include "Window.hpp"
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
        ~Application();
    
    public:
        void run();
    
    private:
        void initSDL();
        void buildWindow();
    
    private:
        // Renderer
        std::unique_ptr<Renderer::Application> m_renderer;

        // Window
        Window::Instance m_window;
    };
}