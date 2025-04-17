#pragma once

#include "Core.hpp"
#include "Grid/Component/BaseProps.hpp"
#include "Grid/Component/Function.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Container
        {
            const std::string TAG_ID = "Container";

            // Props
            struct CHICANE Props : Component::BaseProps
            {
            public:
                std::vector<Reference> items = {}; // Optional
                Component::Function    itemGetter; // Optional
            };

            CHICANE void assertProps(const Props& inProps);

            CHICANE void compileRaw(const Props& inProps);
            CHICANE void compile(const pugi::xml_node& inNode);
        }
    }
}