#include "Application.hpp"

int main(int argc, char *argv[])
{
    try
    {
        std::string windowTitle = "Chicane Editor";

        Engine::Runtime::Renderer::Scene scene;

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