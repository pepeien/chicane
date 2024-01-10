#include "Application.hpp"

namespace Editor
{
    int run()
    {
        try
        {
            std::string windowTitle = "Chicane Editor";

            std::unique_ptr<Engine::Actor> floor = std::make_unique<Engine::Actor>();
            floor->setAbsoluteTranslation(glm::vec3(0.0f, 0.0f, 0.0f));
            floor->setAbsoluteScale(glm::vec3(512.0f, 512.0f, 20.0f));
            floor->setMesh({
                "floor",
                "grid"
            });

            std::shared_ptr<Engine::Level> level = std::make_shared<Engine::Level>();
            level->addActor(floor.get());

            std::unique_ptr<Engine::Core> engineCore = std::make_unique<Engine::Core>(windowTitle, level);
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