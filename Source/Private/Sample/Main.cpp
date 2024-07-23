#include "Main.hpp"

#include "Application.hpp"

int main(int argc, char *argv[])
{
    try
    {
        std::unique_ptr<Application> application = std::make_unique<Application>();
        application->run();
    }
    catch (const std::exception& e)
    {
        LOG_CRITICAL(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}