#include "Application.hpp"

namespace Chicane
{
    namespace Editor
    {
        void initLevel(Level::Instance& outLevel)
        {
            Level::Actor::Instance aircraft;
            aircraft.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
            aircraft.transform.scale       = glm::vec3(1.0f);
            aircraft.transform.rotation    = glm::vec3(0.0f, 0.0f, 0.0f);
            aircraft.mesh.id               = "AirCraft";
            aircraft.texture.id            = "Gray";

            outLevel.addActor(aircraft);
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