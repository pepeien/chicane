#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Grid/Component/BaseProps.hpp"
#include "Chicane/Grid/Component/Function.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ContainerComponent
        {
            const std::string TAG_ID = "Container";

            // Props
            struct Props : Component::BaseProps
            {
            public:
                std::vector<std::any> items = {}; // Optional
                Component::Function   itemGetter; // Optional
            };

            void validate(const Props& inProps);

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}