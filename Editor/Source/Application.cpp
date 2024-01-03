#include "Application.hpp"

namespace Chicane
{
    namespace Editor
    {
        void initLevel(Level::Instance& outLevel)
        {
            Level::Actor::Pawn actor;
            actor.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
            actor.transform.scale       = glm::vec3(512.0f, 512.0f, 2.0f);
            actor.transform.rotation    = glm::vec3(0.0f, 0.0f, 0.0f);
            actor.mesh.id               = "floor";
            actor.texture.id            = "grid";

            outLevel.addActor(actor);
        }

        int run()
        {
            try
            {
                std::string windowTitle = "Chicane Editor";

                Level::Instance level;

                initLevel(level);

                Application application(windowTitle, level);

                application.run();
            }
            catch (const std::exception& e)
            {
                LOG_CRITICAL(e.what());

                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }
    }
}