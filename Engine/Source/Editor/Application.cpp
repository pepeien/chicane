#include "Application.hpp"

namespace Chicane
{
    namespace Editor
    {
        void initScene(Scene::Instance& outScene)
        {
            Scene::Object::Instance triangle;
            triangle.transform.translation = glm::vec3(0.3f, -0.5f, 0.0f);
            triangle.transform.scale       = glm::vec3(0.35f);
            triangle.mesh.id               = "Triangle";
            triangle.texture.id            = "Grid";

            outScene.addObject(triangle);

            Scene::Object::Instance square;
            square.transform.translation = glm::vec3(0.3f, 0.5f, 0.0f);
            square.transform.scale       = glm::vec3(0.35f);
            square.mesh.id               = "Square";
            square.texture.id            = "Grid";

            outScene.addObject(square);

            Scene::Object::Instance square2;
            square2.transform.translation = glm::vec3(0.25f, 0.5f, -0.2f);
            square2.transform.scale       = glm::vec3(0.35f);
            square2.mesh.id               = "Square";
            square2.texture.id            = "Grid";

            outScene.addObject(square2);
        }

        int run()
        {
            try
            {
                std::string windowTitle = "Chicane Editor";

                Scene::Instance scene;

                initScene(scene);

                Application application(windowTitle, scene);

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