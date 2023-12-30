#include "Application.hpp"

namespace Chicane
{
    namespace Editor
    {
        void initLevel(Level::Instance& outLevel)
        {
            Level::Actor::Instance actor;
            actor.transform.translation = glm::vec3(0.0f, -600.0f, 300.0f);
            actor.transform.scale       = glm::vec3(0.5f);
            actor.transform.rotation    = glm::vec3(0.0f, 0.0f, 0.0f);
            actor.mesh.id               = "model-one";
            actor.texture.id            = "gray";

            outLevel.addActor(actor);

            Level::Actor::Instance actor2;
            actor2.transform.translation = glm::vec3(0.0f, 600.0f, 150.0f);
            actor2.transform.scale       = glm::vec3(0.9f);
            actor2.transform.rotation    = glm::vec3(0.0f, 0.0f, 0.0f);
            actor2.mesh.id               = "model-two";
            actor2.texture.id            = "uv";

            outLevel.addActor(actor2);

            Level::Actor::Instance actor3;
            actor3.transform.translation = glm::vec3(0.0f, 600.0f, -400.0f);
            actor3.transform.scale       = glm::vec3(0.9f);
            actor3.transform.rotation    = glm::vec3(0.0f, 0.0f, 0.0f);
            actor3.mesh.id               = "model-three";
            actor3.texture.id            = "grid";

            outLevel.addActor(actor3);

            Level::Actor::Instance actor4;
            actor4.transform.translation = glm::vec3(0.0f, -600.0f, -300.0f);
            actor4.transform.scale       = glm::vec3(100.0f);
            actor4.transform.rotation    = glm::vec3(0.0f, 0.0f, 0.0f);
            actor4.mesh.id               = "model-four";

            outLevel.addActor(actor4);
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