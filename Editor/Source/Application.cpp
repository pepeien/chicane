#include "Application.hpp"

namespace Editor
{
    int run()
    {
        try
        {
            std::unique_ptr<Engine::Actor> floor = std::make_unique<Engine::Actor>();
            floor->setAbsoluteTranslation(glm::vec3(0.0f, 0.0f, 0.0f));
            floor->setAbsoluteScale(glm::vec3(512.0f, 512.0f, 20.0f));
            floor->setMesh({
                "floor",
                "grid"
            });

            std::unique_ptr<Engine::Level> level = std::make_unique<Engine::Level>();
            level->addActor(floor.get());

            Engine::WindowCreateInfo window;
            window.title = "Chicane Editor";

            std::unique_ptr<Engine::Core> engineCore = std::make_unique<Engine::Core>(
                window,
                level.get()
            );
            engineCore->run();
        }
        catch (const std::exception& e)
        {
            LOG_CRITICAL(e.what());

            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
}