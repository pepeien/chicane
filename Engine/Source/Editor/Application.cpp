#include "Application.hpp"

int main(int argc, char *argv[])
{
    try
    {
        std::string windowTitle = "Chicane Editor";

        Engine::Runtime::Core::Application application(windowTitle);

        application.run();
    }
    catch (const std::exception& e)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}