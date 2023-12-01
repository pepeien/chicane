#define GLFW_INCLUDE_VULKAN

#include <string>

#include "Core/Application.h"
#include "Core/Log.h"

int main()
{
    Engine::Core::Application application;
    Engine::Core::Log log;

    try
    {
        application.run();
    }
    catch (const std::exception& e)
    {
        log.critical(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}