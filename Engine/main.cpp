#include "Core/Application.h"

int main()
{
    try
    {
        Engine::Core::Application application;

        application.run();
    }
    catch (const std::exception& e)
    {
        Engine::Core::Log::critical(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}