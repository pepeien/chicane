#pragma once

#include "Core.hpp"
#include "Grid/Component/BaseProps.hpp"

namespace Chicane
{
    namespace Grid
    {
        namespace ProgressBar
        {
            const std::string TAG_ID = "ProgressBar";
    
            // Attributes
            const std::string PERCENTAGE_ATTRIBUTE_NAME = "percentage"; // Goes from `0.0` to `100.0`
            const std::string OVERLAY_ATTRIBUTE_NAME    = "overlay"; // Text that goes inside

            // Types
            struct CHICANE Props : Component::BaseProps
            {
                float       percentage = 0.0f; // Optional
                std::string text       = ""; // Optional
            };
    
            CHICANE void compileRaw(const Props& inProps);
            CHICANE void compile(const pugi::xml_node& inNode);
        }
    }
}