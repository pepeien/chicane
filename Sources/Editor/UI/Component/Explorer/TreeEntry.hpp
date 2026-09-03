#pragma once

#include <vector>

#include <Chicane/Core/Reflection.hpp>
#include <Chicane/Core/String.hpp>

namespace Editor
{
    CH_TYPE(Manual)
    struct ExplorerTreeEntry
    {
    public:
        using List = std::vector<ExplorerTreeEntry>;

    public:
        ExplorerTreeEntry();

    public:
        CH_FIELD()
        Chicane::String name;
        CH_FIELD()
        Chicane::String path;
        CH_FIELD()
        Chicane::String indent;

        CH_FIELD()
        Chicane::String expandState;
        CH_FIELD()
        Chicane::String selectedState;
    };
}
