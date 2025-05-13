#pragma once

#include "Chicane/Core.hpp"
#include "Chicane/Grid/Component/Instance.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE ProgressBar : public Component
        {
        public:
            // Tag
            static constexpr const char* TAG_ID = "ProgressBar";
    
            // Attributes
            static constexpr const char* PERCENTAGE_ATTRIBUTE_NAME = "percentage"; // Goes from `0.0` to `100.0`
            static constexpr const char* OVERLAY_ATTRIBUTE_NAME    = "overlay"; // Text that goes inside

        public:
            ProgressBar(const pugi::xml_node& inNode)
                : Component(inNode)
            {}
        };
    }
}