#define GLFW_INCLUDE_VULKAN

#include "core.h"

int main() {
    Core application;

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