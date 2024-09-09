#pragma once

#include "Chicane/Grid/View.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ContainerComponent
        {
            const std::string TAG_ID = "Container";

            // Props
            struct Props : BaseProps
            {
                std::vector<std::any> items = {}; // Optional
                ComponentFunction     itemGetter; // Optional
            };

            void validate(const Props& inProps);

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}