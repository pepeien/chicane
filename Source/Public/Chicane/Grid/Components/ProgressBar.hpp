#pragma once

#include "Chicane/Grid/Essential.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ProgressBarComponent
        {
            const std::string TAG_ID = "ProgressBar";

            // Types
            struct Props : BaseProps
            {
                float percentage = 0.0f; // Optional
                std::string text = ""; // Optional
            };
    
            // Attributes
            const std::string PERCENTAGE_ATTRIBUTE_NAME = "percentage"; // Goes from `0.0` to `100.0`
            const std::string OVERLAY_ATTRIBUTE_NAME    = "overlay"; // Text that goes inside
    
            void compileRaw(const Props& inProps);
            void compile(const pugi::xml_node& inNode);
        }
    }
}