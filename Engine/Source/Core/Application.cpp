#include "Application.hpp"

namespace Chicane
{
    Application::Application(
        const std::string& inWindowTitle,
        const Level::Instance& inLevel
    )
        : m_renderer(nullptr),
        m_window({ nullptr, inWindowTitle, 0, 0 })
    {
        initSDL();
        buildWindow();

        m_renderer = std::make_unique<Renderer::Application>(
            m_window,
            inLevel
        );
    }

    Application::~Application()
    {
        // Window
        SDL_DestroyWindow(m_window.instance);
        SDL_Quit();
    }

    void Application::run()
    {
        bool shouldClose = false;

        SDL_Event event;

        while (shouldClose == false)
        {
            while(SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_MOUSEBUTTONUP:
                    FileSystem::Pak::write(
                        { FileSystem::Paths::contentDir() + "Models/apple.obj" },
                        FileSystem::Paths::contentDir() + "Models/apple.apk"
                    );

                    break;
                case SDL_QUIT:
                    shouldClose = true;

                    break;

                default:
                    m_renderer->process(event);

                    break;
                }
            }

            m_renderer->render();
            m_renderer->updateStats();
        }
    }

    void Application::initSDL()
    {
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            throw std::runtime_error(SDL_GetError());
        }
    }

    void Application::buildWindow()
    {
        Window::init(m_window);
    }
}