#pragma once

#include "Chicane/Grid/Components/Container.hpp"
#include "Chicane/Grid/View.hpp"
#include "Chicane/Grid/Style.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace GridComponent
        {
            const std::string TAG_ID = "Grid";

            // Attributes
            constexpr auto ITEM_WIDTH_ATTRIBUTE_NAME  = "itemWidth";
            constexpr auto ITEM_HEIGHT_ATTRIBUTE_NAME = "itemHeight";

            // Types
            struct Props : BaseProps
            {
            public:
                Vec<2, float>         itemSize = {}; // Optional
                std::vector<std::any> items    = {}; // Optional
                ComponentFunction     itemGetter; // Optional
            };

            void validate(const Props& inProps);

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}