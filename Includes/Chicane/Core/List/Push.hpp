#pragma once

#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/List/Push/Strategy.hpp"

namespace Chicane
{
    template <typename T>
    struct ListPush
    {
    public:
        using Predicate = std::function<bool(T)>;

    public:
        Predicate        predicate = nullptr;
        ListPushStrategy strategy  = ListPushStrategy::Back;
    };
}