#include "Main.hpp"

#include <Chicane/Core/Log.hpp>

#include "Application.hpp"

int main(int argc, char* argv[])
{
    try
    {
        Application();
    }
    catch (const std::exception& e)
    {
        Chicane::Log::critical(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}