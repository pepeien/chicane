#pragma once

#include "Chicane/Grid.hpp"
#include "Chicane/Grid/Component.hpp"

namespace Chicane
{
    namespace Grid
    {
        class CHICANE_GRID ProgressBar : public Component
        {
        public:
            // Tag
            static inline constexpr const char* TAG_ID = "ProgressBar";

            // Attributes
            static inline constexpr const char* PERCENTAGE_ATTRIBUTE_NAME =
                "percentage"; // Goes from `0.0` to `100.0`
            static inline constexpr const char* OVERLAY_ATTRIBUTE_NAME = "overlay"; // Text that goes inside

        public:
            ProgressBar(const pugi::xml_node& inNode)
                : Component(inNode)
            {}
        };
    }
}