#include "Core.hpp"

namespace Engine
{
    Core::Core(
        const WindowCreateInfo& inWindowCreateInfo,
        Level* inLevel
    )
    {
        window   = std::make_unique<Window>(inWindowCreateInfo);
        m_level    = inLevel;
        m_renderer = std::make_unique<Renderer>(window.get(), m_level);
    }

    void Core::run()
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
                    m_renderer->onEvent(event);

                    break;
                }
            }

            m_renderer->render();
            m_renderer->updateStats();
        }
    }
}