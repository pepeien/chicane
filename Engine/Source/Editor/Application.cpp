#include "Application.hpp"

void initScene(Engine::Runtime::Renderer::Scene& outScene)
{
    std::vector<Engine::Runtime::Renderer::Vertex2D*> triangleVertices;
    triangleVertices.resize(3);

    triangleVertices[0] = new Engine::Runtime::Renderer::Vertex2D();
    triangleVertices[0]->position = glm::vec2(0.0f, -0.15f);
    triangleVertices[0]->color    = glm::vec3(0.0f, 1.0f, 0.0f);

    triangleVertices[1] = new Engine::Runtime::Renderer::Vertex2D();
    triangleVertices[1]->position = glm::vec2(0.15f, 0.15f);
    triangleVertices[1]->color    = glm::vec3(0.0f, 1.0f, 0.0f);

    triangleVertices[2] = new Engine::Runtime::Renderer::Vertex2D();
    triangleVertices[2]->position = glm::vec2(-0.15f, 0.15f);
    triangleVertices[2]->color    = glm::vec3(0.0f, 1.0f, 0.0f);

    Engine::Runtime::Renderer::SceneObject<Engine::Runtime::Renderer::Vertex2D> triangle;
    triangle.position = glm::vec3(0.0f);
    triangle.vertices = triangleVertices; 

    outScene.addObject(triangle);

    std::vector<Engine::Runtime::Renderer::Vertex2D*> squareVertices;
    squareVertices.resize(6);

    squareVertices[0] = new Engine::Runtime::Renderer::Vertex2D();
    squareVertices[0]->position = glm::vec2(-0.15f,  0.15f);
    squareVertices[0]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

    squareVertices[1] = new Engine::Runtime::Renderer::Vertex2D();
    squareVertices[1]->position = glm::vec2(-0.15f, -0.15f);
    squareVertices[1]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

    squareVertices[2] = new Engine::Runtime::Renderer::Vertex2D();
    squareVertices[2]->position = glm::vec2(0.15f, -0.15f);
    squareVertices[2]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

    squareVertices[3] = new Engine::Runtime::Renderer::Vertex2D();
    squareVertices[3]->position = glm::vec2(0.15f, -0.15f);
    squareVertices[3]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

    squareVertices[4] = new Engine::Runtime::Renderer::Vertex2D();
    squareVertices[4]->position = glm::vec2(0.15f, 0.15f);
    squareVertices[4]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

    squareVertices[5] = new Engine::Runtime::Renderer::Vertex2D();
    squareVertices[5]->position = glm::vec2(-0.15f, 0.15f);
    squareVertices[5]->color    = glm::vec3(0.0f, 0.0f, 1.0f);

    Engine::Runtime::Renderer::SceneObject<Engine::Runtime::Renderer::Vertex2D> square;
    square.position = glm::vec3(5.0f);
    square.vertices = squareVertices;

    outScene.addObject(square);
}

int main(int argc, char *argv[])
{
    try
    {
        std::string windowTitle = "Chicane Editor";

        Engine::Runtime::Renderer::Scene scene;

        initScene(scene);

        Engine::Runtime::Core::Application application(windowTitle, scene);

        application.run();
    }
    catch (const std::exception& e)
    {
        Engine::Runtime::Core::Log::critical(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}