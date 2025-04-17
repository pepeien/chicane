#pragma once

#include "Core.hpp"
#include "Core/Math/Vec.hpp"
#include "Grid/Component/BaseProps.hpp"
#include "Grid/Component/Variable.hpp"
#include "Grid/Component/Function.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace List
        {
            const std::string TAG_ID = "List";

            // Attributes
            static constexpr const char* ITEM_WIDTH_ATTRIBUTE_NAME  = "itemWidth";
            static constexpr const char* ITEM_HEIGHT_ATTRIBUTE_NAME = "itemHeight";

            // Types
            struct CHICANE Props : Component::BaseProps
            {
            public:
                Vec<2, float>          itemSize = {}; // Optional
                std::vector<Reference> items    = {}; // Optional
                Component::Function    itemGetter; // Optional
            };

            CHICANE void assertProps(const Props& inProps);

            CHICANE void compileRaw(const Props& inProps);
            CHICANE void compile(const pugi::xml_node& inNode);
        }
    }
}