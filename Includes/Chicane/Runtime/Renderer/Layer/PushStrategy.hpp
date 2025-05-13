#pragma once

#include "Chicane/Core.hpp"

namespace Chicane
{
    namespace Layer
    {
        enum class PushStrategy : std::uint8_t
        {
            Front,
            Back,
            BeforeLayer, // Require to have the ref layer `ID`
            AfterLayer   // Require to have the ref layer `ID`
        };
    }
}