#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid/Component/Child.hpp"
#include "Chicane/Grid/Component/Function.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Component
        {
            struct BaseProps
            {
            public:
                bool                             bIsVisible = true; // Optional
                std::string                      id         = ""; // Required
                Style                            style      = {}; // Optional
                Children              children   = Component::Children(pugi::xml_node_iterator(), pugi::xml_node_iterator()); // Optional
                std::vector<Component::Function> _renderers = {}; // [Internal Use]
            };
        }
    }
}