#pragma once

#include "Game.hpp"
#include "Renderer.hpp"

#include "Core/FileSystem.hpp"
#include "Core/Helper.hpp"
#include "Core/Log.hpp"
#include "Core/Window.hpp"

namespace Engine
{
    class Core
    {
    public:
        Core(
            const WindowCreateInfo& inWindowCreateInfo,
            Level* inLevel
        );

    public:
        void run();

    public:
        std::unique_ptr<Window> window;

    private:
        Level* m_level;

        std::unique_ptr<Renderer> m_renderer;
    };
}