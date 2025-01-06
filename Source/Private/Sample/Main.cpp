#include "Main.hpp"

#include "Application.hpp"

int main(int argc, char *argv[])
{
    try
    {
        std::unique_ptr<Application> application = std::make_unique<Application>();
    }
    catch (const std::exception& e)
    {
        Chicane::Log::critical(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}