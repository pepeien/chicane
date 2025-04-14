#pragma once

#include "Chicane.hpp"
#include "Chicane/Core/Math/Vec.hpp"
#include "Chicane/Grid/Component/BaseProps.hpp"
#include "Chicane/Grid/Component/Variable.hpp"
#include "Chicane/Grid/Component/Function.hpp"

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
            struct Props : Component::BaseProps
            {
            public:
                Vec<2, float>          itemSize = {}; // Optional
                std::vector<Reference> items    = {}; // Optional
                Component::Function    itemGetter; // Optional
            };

            void assertProps(const Props& inProps);

            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}