#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ProgressBarComponent
        {
            const float MIN_PERCENTAGE = 0.0f;
            const float MAX_PERCENTAGE = 100.0f;
    
            const std::string TAG_ID = "ProgressBar";
    
            // Attributes
            const std::string PERCENTAGE_ATTRIBUTE_NAME = "percentage"; // Goes from `0.0` to `100.0`
            const std::string OVERLAY_ATTRIBUTE_NAME    = "overlay"; // Text that goes inside
    
            void compile(pugi::xml_node& outNode);
        }
    }
}