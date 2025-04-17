#pragma once

#include "Core.hpp"
#include "Grid/Component/Child.hpp"
#include "Grid/Component/Event.hpp"
#include "Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Component
        {
            struct CHICANE BaseProps
            {
            public:
                bool                         bIsVisible = true; // Optional
                std::string                  id         = ""; // Required
                Style                        style      = {}; // Optional
                Children                     children   = Children(pugi::xml_node_iterator(), pugi::xml_node_iterator()); // Optional
                std::vector<Event::Function> _renderers = {}; // [Internal Use]
            };
        }
    }
}