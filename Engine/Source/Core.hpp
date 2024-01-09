#pragma once

#include "Core/FileSystem.hpp"
#include "Core/Helper.hpp"
#include "Core/Log.hpp"
#include "Core/Window.hpp"

#include "Game/Actor.hpp"
#include "Game/Controller.hpp"
#include "Game/Level.hpp"

namespace Engine
{
    class Renderer;

    class Core
    {
    public:
        Core(
            const std::string& inWindowTitle,
            std::shared_ptr<Level> inLevel
        );
        ~Core();
    
    public:
        void run();
    
    private:
        void initSDL();
        void buildWindow();
    
    private:
        Window::Instance m_window;

        std::unique_ptr<Renderer> m_renderer;

        std::shared_ptr<Level> m_level;
    };
}