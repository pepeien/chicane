#include "Application.hpp"

int main(int argc, char *argv[])
{
    try
    {
        std::string windowTitle = "Chicane Editor";

        Engine::Core::Application application(windowTitle);

        application.run();
    }
    catch (const std::exception& e)
    {
        Engine::Core::Log::critical(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}