#include "Main.hpp"

#include "Application.hpp"
#include "Chicane/Core/Log.hpp"

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