#pragma once

#include <functional>

#include "Chicane/Core.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    template <typename T = void*>
    struct CHICANE_CORE EventSubscriptionState
    {
    public:
        bool                               bIsCompleted = false;
        std::function<void(T)>             next         = nullptr;
        std::function<void(const String&)> error        = nullptr;
        std::function<void()>              complete     = nullptr;
    };
}
