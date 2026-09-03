#pragma once

#include <vector>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    struct HeaderMenuItem
    {
    public:
        using List = std::vector<HeaderMenuItem>;

    public:
        HeaderMenuItem();

    public:
        CH_FIELD()
        Chicane::String label;
        CH_FIELD()
        Chicane::String shortcut;
        CH_FIELD()
        Chicane::String action;
        CH_FIELD()
        List children;
    };
}
