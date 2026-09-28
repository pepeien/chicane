#pragma once

#include <Chicane/Core/String.hpp>

namespace Editor
{
    struct ConsoleCommand
    {
    public:
        Chicane::String name;
        Chicane::String method;

        bool bTakesArgument = false;
    };
}
