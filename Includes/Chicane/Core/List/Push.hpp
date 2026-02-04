#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/List/Push/Strategy.hpp"

namespace Chicane
{
    template <typename T>
    struct CHICANE_CORE ListPush
    {
    public:
        using Predicate = std::function<bool(T)>;

    public:
        Predicate        predicate = nullptr;
        ListPushStrategy strategy  = ListPushStrategy::Back;
    };
}