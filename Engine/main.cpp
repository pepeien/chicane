#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <ostream>

#include "Core/Application.h"

int main()
{
    Engine::Core::Application application;

    try
    {
        application.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}