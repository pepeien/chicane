#include "Application.hpp"

namespace Engine
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
            square.texture.id            = "UV";

            outScene.addObject(square);
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
                Log::critical(e.what());

                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }
    }
}