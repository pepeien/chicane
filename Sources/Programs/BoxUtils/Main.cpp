#include "Main.hpp"

#include <Chicane/Core/Log.hpp>

#include "Program.hpp"

int main(int argc, char* argv[])
{
    try
    {
        Program().exec(argc, argv);
    }
    catch (const std::exception& e)
    {
        Chicane::Log::critical(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}