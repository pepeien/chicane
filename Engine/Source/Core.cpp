#include "Core.hpp"

namespace Engine
{
    Core::Core(
        const WindowCreateInfo& inWindowCreateInfo,
        Level* inLevel
    )
    {
        window     = std::make_unique<Window>(inWindowCreateInfo);
        m_level    = inLevel;
        m_renderer = std::make_unique<Renderer>(window.get(), m_level);
    }

    void Core::run()
    {
        SDL_Event event;

        bool shouldClose = false;
        while (shouldClose == false)
        {
            while(SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    shouldClose = true;

                    break;
                }

                window->onEvent(event);
                m_renderer->onEvent(event);
            }

            m_renderer->render();
        }
    }
}