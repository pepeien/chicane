#include "Application.hpp"

namespace Engine
{
	namespace Editor
	{
        void initScene(Runtime::Renderer::Scene::Instance& outScene)
        {
            std::vector<Runtime::Renderer::Vertex2D*> triangleVertices;
            triangleVertices.resize(3);

            triangleVertices[0] = new Runtime::Renderer::Vertex2D();
            triangleVertices[0]->position = glm::vec2(0.0f, -1.0f);
            triangleVertices[0]->color    = glm::vec3(1.0f, 0.0f, 0.0f);

            triangleVertices[1] = new Runtime::Renderer::Vertex2D();
            triangleVertices[1]->position = glm::vec2(1.0f, 1.0f);
            triangleVertices[1]->color    = glm::vec3(1.0f, 0.0f, 0.0f);

            triangleVertices[2] = new Runtime::Renderer::Vertex2D();
            triangleVertices[2]->position = glm::vec2(-1.0f, 1.0f);
            triangleVertices[2]->color    = glm::vec3(1.0f, 0.0f, 0.0f);

            Runtime::Renderer::Scene::Object<Runtime::Renderer::Vertex2D> triangle;
            triangle.vertices              = triangleVertices;
            triangle.transform.translation = glm::vec3(-0.5f, -0.5f, 0.0f);
            triangle.transform.scale       = glm::vec3(0.25f);

            outScene.addObject(triangle);

            std::vector<Runtime::Renderer::Vertex2D*> squareVertices;
            squareVertices.resize(6);

            squareVertices[0] = new Runtime::Renderer::Vertex2D();
            squareVertices[0]->position = glm::vec2(-0.5f, 1.0f);
            squareVertices[0]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

            squareVertices[1] = new Runtime::Renderer::Vertex2D();
            squareVertices[1]->position = glm::vec2(-0.5f, -1.0f);
            squareVertices[1]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

            squareVertices[2] = new Runtime::Renderer::Vertex2D();
            squareVertices[2]->position = glm::vec2(0.5f, -1.0f);
            squareVertices[2]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

            squareVertices[3] = new Runtime::Renderer::Vertex2D();
            squareVertices[3]->position = glm::vec2(0.5f, -1.0f);
            squareVertices[3]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

            squareVertices[4] = new Runtime::Renderer::Vertex2D();
            squareVertices[4]->position = glm::vec2(0.5f, 1.0f);
            squareVertices[4]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

            squareVertices[5] = new Runtime::Renderer::Vertex2D();
            squareVertices[5]->position = glm::vec2(-0.5f, 1.0f);
            squareVertices[5]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

            Runtime::Renderer::Scene::Object<Runtime::Renderer::Vertex2D> square;
            square.vertices              = squareVertices;
            square.transform.translation = glm::vec3(0.5f, 0.5f, 0.0f);
            square.transform.scale       = glm::vec3(0.25f);

            outScene.addObject(square);
        }

        int run()
        {
            try
            {
                std::string windowTitle = "Chicane Editor";

                Runtime::Renderer::Scene::Instance scene;

                initScene(scene);

                Runtime::Core::Application application(windowTitle, scene);

                application.run();
            }
            catch (const std::exception& e)
            {
                Runtime::Core::Log::critical(e.what());

                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }
    }
}