#include "Main.hpp"

#include <vector>

#include <Chicane/Core/FileSystem/Path.hpp>
#include <Chicane/Core/Log.hpp>
#include <Chicane/Core/String.hpp>

#include "Editor/Application.hpp"

std::vector<Chicane::FileSystem::Path> parseModules(int argc, char* argv[])
{
    std::vector<Chicane::FileSystem::Path> modules;

    for (int i = 1; i < argc; ++i)
    {
        const Chicane::String arg = argv[i] ? argv[i] : "";
        if (!arg.equals("--module"))
        {
            continue;
        }

        if (i + 1 >= argc || !argv[i + 1])
        {
            Chicane::Log::warning("--module requires a path");

            break;
        }

        modules.emplace_back(argv[++i]);
    }

    return modules;
}

int main(int argc, char* argv[])
{
    try
    {
        Editor::Application(parseModules(argc, argv));
    }
    catch (const std::exception& e)
    {
        Chicane::Log::critical(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
