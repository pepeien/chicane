#include "Application.hpp"

namespace Chicane
{
    namespace Editor
    {
        void initLevel(Level::Instance& outLevel)
        {
            Level::Actor::Instance cube;
            cube.transform.translation = glm::vec3(-0.1f, -0.5f, 0.0f);
            cube.transform.scale       = glm::vec3(0.05f);
            cube.transform.rotation    = glm::vec3(-90.0f, -40.0f, 0.0f);
            cube.mesh.id               = "Cube";
            cube.texture.id            = "Grid";

            outLevel.addActor(cube);

            Level::Actor::Instance aircraft;
            aircraft.transform.translation = glm::vec3(0.0f, 0.0f, 0.0f);
            aircraft.transform.scale       = glm::vec3(0.0005f);
            aircraft.transform.rotation    = glm::vec3(-90.0f, -40.0f, 0.0f);
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