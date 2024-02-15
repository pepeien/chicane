#include "Editor/Main.hpp"

#include "Editor/Editor.hpp"

int main(int argc, char *argv[])
{
    try
    {
        Chicane::Editor::run();
    }
    catch (const std::exception& e)
    {
        LOG_CRITICAL(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}