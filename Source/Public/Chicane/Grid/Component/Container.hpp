#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid/Component/BaseProps.hpp"
#include "Chicane/Grid/Component/Function.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace Container
        {
            const std::string TAG_ID = "Container";

            // Props
            struct Props : Component::BaseProps
            {
            public:
                std::vector<Reference> items = {}; // Optional
                Component::Function    itemGetter; // Optional
            };

            void assertProps(const Props& inProps);

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}