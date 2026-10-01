#pragma once

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component/Status.hpp"

namespace Chicane
{
    namespace Grid
    {
        struct CHICANE_GRID StylePseudoClass
        {
        public:
            const char*     token  = nullptr;
            ComponentStatus status = ComponentStatus::None;
        };
    }
}
