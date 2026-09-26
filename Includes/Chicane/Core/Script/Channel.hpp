#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Core/Script/Queue.hpp"

namespace Chicane
{
    namespace Script
    {
        class CHICANE_CORE Channel
        {
        public:
            static Queue& sToView();
            static Queue& sToScene();
        };
    }
}
