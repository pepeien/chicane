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
                FileSystem::Pak::WriteInfo writeInfo;

                switch (event.type)
                {
                case SDL_MOUSEBUTTONUP:
                    writeInfo.type       = FileSystem::Pak::Type::Mesh;
                    writeInfo.name       = "Floor";
                    writeInfo.outputPath = FileSystem::Paths::contentDir() + "Pak/";
                    writeInfo.entries.push_back({
                        FileSystem::Pak::EntryType::Model,
                        FileSystem::Paths::contentDir() + "Models/floor.obj"
                    });
                    writeInfo.entries.push_back({
                        FileSystem::Pak::EntryType::Texture,
                        FileSystem::Paths::contentDir() + "Textures/Base/grid.png"
                    });

                    FileSystem::Pak::write(writeInfo);

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